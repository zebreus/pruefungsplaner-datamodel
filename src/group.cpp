#include <group.h>

Group::Group(QObject* parent) : SerializableDataObject(parent) {}

QString Group::getName() const {
  return name;
}

void Group::setName(const QString& name) {
  if (this->name == name)
    return;

  this->name = name;
  emit nameChanged(name);
}

bool Group::getSelected() const {
  return selected;
}

void Group::setSelected(const bool selected) {
  if (selected == this->selected)
    return;

  this->selected = selected;
  emit selectedChanged(selected);
}

unsigned int Group::getExamsPerDay() const {
  return examsPerDay;
}

void Group::setExamsPerDay(unsigned int examsPerDay) {
  if (examsPerDay == this->examsPerDay)
    return;

  this->examsPerDay = examsPerDay;
  emit examsPerDayChanged(examsPerDay);
}

void Group::fromJsonObject(const QJsonObject& content) {
  simpleValuesFromJsonObject(content);
}

QJsonObject Group::toJsonObject() const {
  return recursiveToJsonObject();
}
