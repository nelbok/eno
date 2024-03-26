#include <eno/io/Eno.hpp>

#include <QtCore/QFileInfo>
#include <QtCore/QDataStream>
#include <QtCore/QSaveFile>

#include <eno/data/Material.hpp>
#include <eno/data/Object.hpp>
#include <eno/data/Project.hpp>
#include <eno/data/Scene.hpp>

namespace eno {

void Eno::save() {
	assert(_project);

	QSaveFile file(_filePath);
	if (!file.open(QIODevice::WriteOnly)) {
		_result = Result::Error;
		return;
	}

	QDataStream stream(&file);
	stream.setVersion(QDataStream::Qt_5_15);

	// Write version
	stream << Eno::fileVersion;

	// Write project
	{ stream << _project->name(); }

	// Write tags
	{
		const auto& tags = _project->tags();
		int nbTags = tags.count();
		stream << nbTags;
		for (const auto& tag : tags) {
			if (isInterruptionRequested()) {
				break;
			}
			stream << tag;
		}
	}

	// Write textures
	{
		const auto& textures = _project->textures();
		int nbTextures = textures.count();
		stream << nbTextures;
		for (auto* texture : textures) {
			if (isInterruptionRequested()) {
				break;
			}
			stream << texture->uuid();
			stream << texture->name();
			stream << texture->size();
			stream << texture->data();
			stream << texture->invertX();
			stream << texture->invertY();
		}
	}

	// Write materials
	{
		const auto& materials = _project->materials();
		int nbMaterials = materials.count();
		stream << nbMaterials;
		for (auto* material : materials) {
			if (isInterruptionRequested()) {
				break;
			}
			stream << material->uuid();
			stream << material->name();
			stream << material->diffuse();
			stream << (material->diffuseMap() ? material->diffuseMap()->uuid() : QUuid());
			stream << material->opacity();
			stream << (material->opacityMap() ? material->opacityMap()->uuid() : QUuid());
		}
	}

	// Write scene
	{
		auto* scene = _project->scene();

		// Write min/max
		stream << scene->min() << scene->max();

		// Write scene
		const auto& objects = scene->objects();
		int nbObjects = objects.count();
		stream << nbObjects;
		for (auto* object : objects) {
			if (isInterruptionRequested()) {
				break;
			}
			stream << object->uuid();
			stream << object->position();
			stream << object->material()->uuid();
		}
	}

	if (isInterruptionRequested()) {
		_result = Result::Canceled;
		return;
	}

	if (file.commit()) {
		_project->setFilePath(_filePath);
		_project->setIsModified(false);

		_result = Result::Success;
	} else {
		_result = Result::Error;
	}
}

void Eno::load() {
	assert(_project);

	QFile file(_filePath);
	if (!file.open(QIODevice::ReadOnly)) {
		_result = Result::Error;
		return;
	}

	// Reset and begin transaction
	_project->blockSignals(true);
	auto* scene = _project->scene();
	scene->blockSignals(true);
	_project->reset();
	_project->setFilePath(_filePath);

	QDataStream stream(&file);
	stream.setVersion(QDataStream::Qt_5_15);

	// Read version
	auto version = 0u;
	stream >> version;
	if (!(Eno::fileVersion >= version && version > 0u)) {
		_result = Result::Error;
		_project->reset();
		file.close();
		_project->blockSignals(false);
		_project->setIsModified(false);
		return;
	}

	try {
		// Read project
		{
			QString name;
			stream >> name;
			_project->setName(name);
		}

		// Read tags
		{
			int nb = 0;
			stream >> nb;
			for (int i = 0; i < nb; ++i) {
				if (isInterruptionRequested()) {
					break;
				}
				QString tag;
				stream >> tag;
				_project->add({ tag });
			}
		}

		// Read textures
		QMap<QUuid, Texture*> textureLinks;
		{
			int nb = 0;
			stream >> nb;
			QList<Texture*> textures;
			textures.reserve(nb);
			for (int i = 0; i < nb; ++i) {
				if (isInterruptionRequested()) {
					break;
				}
				QUuid uuid;
				QString name;
				QSize size;
				QByteArray data;
				bool invertX{ false };
				bool invertY{ false };
				stream >> uuid;
				stream >> name;
				stream >> size;
				stream >> data;
				stream >> invertX;
				stream >> invertY;

				// Create texture
				auto* texture = new Texture(uuid, _project);
				texture->setName(name);
				texture->set(size, data);
				auto img = texture->image();
				img.mirror(invertX, invertY);
				texture->set(img);
				texture->setInvertX(invertX);
				texture->setInvertY(invertY);
				textures.append(texture);
				textureLinks.insert(uuid, texture);
			}
			_project->add(textures);
		}

		// Read materials
		QMap<QUuid, Material*> materialLinks;
		{
			int nb = 0;
			stream >> nb;
			QList<Material*> materials;
			materials.reserve(nb);
			for (int i = 0; i < nb; ++i) {
				if (isInterruptionRequested()) {
					break;
				}
				QUuid uuid;
				QString name;
				QColor diffuse;
				QUuid diffuseUuid;
				float opacity{ 1.0 };
				QUuid opacityUuid;
				stream >> uuid;
				stream >> name;
				stream >> diffuse;
				stream >> diffuseUuid;
				stream >> opacity;
				stream >> opacityUuid;

				// Create material
				auto* material = new Material(uuid, _project);
				material->setName(name);
				material->setDiffuse(diffuse);
				if (!diffuseUuid.isNull()) {
					material->setDiffuseMap(textureLinks.value(diffuseUuid));
				}
				material->setOpacity(opacity);
				if (!opacityUuid.isNull())
					material->setOpacityMap(textureLinks.value(opacityUuid));
				materials.append(material);
				materialLinks.insert(uuid, material);
			}
			_project->add(materials);
		}

		// Read scene
		{
			// Write min/max
			QPoint min;
			QPoint max;
			stream >> min >> max;
			scene->setMin(min);
			scene->setMax(max);

			// Write scene
			int nb = 0;
			stream >> nb;
			QList<Object*> objects;
			objects.reserve(nb);
			for (int i = 0; i < nb; ++i) {
				if (isInterruptionRequested()) {
					break;
				}

				QUuid uuid;
				QVector3D pos;
				QUuid mUuid;
				stream >> uuid;
				stream >> pos;
				stream >> mUuid;

				// Create object
				Object* object = nullptr;
				object = new Object(uuid, _project);
				object->setPosition(pos);
				object->setMaterial(materialLinks.value(mUuid));
				objects.append(object);
			}
			scene->add(objects);
		}
		_result = Result::Success;
	} catch (...) {
		_result = Result::Error;
		_project->reset();
	}
	file.close();

	if (isInterruptionRequested()) {
		_project->reset();
		_result = Result::Canceled;
	}

	scene->blockSignals(false);
	_project->blockSignals(false);
	_project->setIsModified(false);
}
} // namespace eno
