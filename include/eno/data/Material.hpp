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
	// clang-format on

private:
	QString _name{};
	QColor _diffuse{};
	Texture* _diffuseMap{};

signals:
	void nameUpdated();
	void diffuseUpdated();
	void diffuseMapUpdated();
};
} // namespace eno
