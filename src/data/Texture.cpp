#include <eno/data/Texture.hpp>

#include <eno/data/Project.hpp>

namespace eno {
Texture::Texture(Project* project)
	: Item(project) {}

Texture::Texture(const QUuid& uuid, Project* project)
	: Item(uuid, project) {}

void Texture::setName(const QString& name) {
	if (_name != name) {
		_name = name;
		_project->setIsModified(true);
		emit nameUpdated();
	}
}

QPixmap Texture::pixmap() const {
	return QPixmap::fromImage(image());
}

QImage Texture::image() const {
	QImage img(reinterpret_cast<const uchar*>(_data.data()), _size.width(), _size.height(), QImage::Format_RGBA8888);
	img.mirror();
	return img;
}

void Texture::set(const QSize& size, const QByteArray& data) {
	_size = size;
	_data = data;
	_invertX = false;
	_invertY = false;
	_project->setIsModified(true);
	emit sizeUpdated();
	emit dataUpdated();
	emit invertXUpdated();
	emit invertYUpdated();
}

void Texture::set(const QImage& img) {
	assert(!img.isNull());

	// Image into byte array
	QByteArray data;

	for (int y = 0; y < img.height(); ++y) {
		QByteArray line;
		line.resize(img.width() * 4); // RGBA8
		for (int x = 0; x < img.width(); ++x) {
			const auto& c = img.pixelColor(x, img.height() - y - 1);
			int offset = x * 4;
			line.data()[offset + 0] = char(c.red());
			line.data()[offset + 1] = char(c.green());
			line.data()[offset + 2] = char(c.blue());
			line.data()[offset + 3] = char(c.alpha());
		}
		data += line;
	}

	set(img.size(), data);
}

void Texture::set(const QPixmap& pixmap) {
	assert(!pixmap.isNull());
	set(pixmap.toImage());
}

void Texture::setInvertX(bool value) {
	setInvert(value, _invertY);
}

void Texture::setInvertY(bool value) {
	setInvert(_invertX, value);
}

void Texture::setInvert(bool invertX, bool invertY) {
	bool invertXChanged = invertX != _invertX;
	bool invertYChanged = invertY != _invertY;
	if (invertXChanged || invertYChanged) {
		QImage img = image();
		img.mirror(invertXChanged, invertYChanged);
		set(img);

		_invertX = invertX;
		_invertY = invertY;

		// emit if the value changed
		if (invertXChanged)
			emit invertXUpdated();
		if (invertYChanged)
			emit invertYUpdated();
	}
}

} // namespace eno
