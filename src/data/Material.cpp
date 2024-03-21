#include <eno/data/Material.hpp>

#include <eno/data/Project.hpp>

namespace eno {
Material::Material(Project* project)
	: Item(project) {}

Material::Material(const QUuid& uuid, Project* project)
	: Item(uuid, project) {}

Material::~Material() {
	setDiffuseMap(nullptr);
	setOpacityMap(nullptr);
}

void Material::setName(const QString& name) {
	if (_name != name) {
		_name = name;
		_project->setIsModified(true);
		emit nameUpdated();
	}
}

void Material::setDiffuse(const QColor& color) {
	if (_diffuse != color) {
		_diffuse = color;
		_project->setIsModified(true);
		emit diffuseUpdated();
	}
}

void Material::setDiffuseMap(Texture* diffuseMap) {
	if (_diffuseMap != diffuseMap) {
		// Refcount
		if (_diffuseMap)
			_diffuseMap->decreaseRefCount();
		if (diffuseMap)
			diffuseMap->increaseRefCount();

		_diffuseMap = diffuseMap;
		_project->setIsModified(true);
		emit diffuseMapUpdated();
	}
}

void Material::setOpacity(float opacity) {
	if (_opacity != opacity) {
		_opacity = opacity;
		_project->setIsModified(true);
		emit opacityUpdated();
	}
}

void Material::setOpacityMap(Texture* opacityMap) {
	if (_opacityMap != opacityMap) {
		// Refcount
		if (_opacityMap)
			_opacityMap->decreaseRefCount();
		if (opacityMap)
			opacityMap->increaseRefCount();

		_opacityMap = opacityMap;
		_project->setIsModified(true);
		emit opacityMapUpdated();
	}
}
} // namespace eno
