#pragma once

#include <QtGui/QColor>

#include <eno/data/Container.hpp>

// For Q_PROPERTY
#include <eno/data/Texture.hpp>

namespace eno {
class Project;

class Material : public Item {
	Q_OBJECT
	Q_PROPERTY(QUuid uuid READ uuid NOTIFY uuidUpdated)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameUpdated)
	Q_PROPERTY(QColor diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseUpdated)
	Q_PROPERTY(Texture* diffuseMap READ diffuseMap WRITE setDiffuseMap NOTIFY diffuseMapUpdated)
	Q_PROPERTY(float opacity READ opacity WRITE setOpacity NOTIFY opacityUpdated)
	Q_PROPERTY(Texture* opacityMap READ opacityMap WRITE setOpacityMap NOTIFY opacityMapUpdated)

public:
	Material(Project* project);
	Material(const QUuid& uuid, Project* project);
	virtual ~Material();

	// clang-format off
	const QString& name() const { return _name; }
	void setName(const QString& name);

	const QColor& diffuse() const { return _diffuse; }
	void setDiffuse(const QColor& color);

	Texture* diffuseMap() const { return _diffuseMap; }
	void setDiffuseMap(Texture* diffuseMap);

	float opacity() const { return _opacity; }
	void setOpacity(float opacity);

	Texture* opacityMap() const { return _opacityMap; }
	void setOpacityMap(Texture* opacityMap);
	// clang-format on

private:
	QString _name{};
	QColor _diffuse{};
	Texture* _diffuseMap{ nullptr };
	float _opacity{ 1.0 };
	Texture* _opacityMap{ nullptr };

signals:
	void nameUpdated();
	void diffuseUpdated();
	void diffuseMapUpdated();
	void opacityUpdated();
	void opacityMapUpdated();
};
} // namespace eno
