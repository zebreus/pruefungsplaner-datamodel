#ifndef PLANCSVHELPER_H
#define PLANCSVHELPER_H

#include <plan.h>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>
#include <QTemporaryDir>

/**
 *  @class PlanCsvHelper
 *  @brief A helper for converting plans to csv files
 *
 *  A helper for converting plans to the csv files required by sp-automatisch
 * and for converting the resulting files from sp-automatisch to a Plan object.
 */
class PlanCsvHelper {
 private:
  QString basePath;
  QScopedPointer<QTemporaryDir> temporaryDirectory;

  QFile examsIntervalsFile;
  QFile examsFile;
  QFile groupsExamsFile;
  QFile groupsExamsPrefFile;

  QFile planningExamsResultFile;
  QFile groupsExamsResultFile;

  static constexpr char const* blockNames[108] = {
      "MO1_1", "MO1_2", "MO1_3", "MO1_4", "MO1_5", "MO1_6", "DI1_1", "DI1_2",
      "DI1_3", "DI1_4", "DI1_5", "DI1_6", "MI1_1", "MI1_2", "MI1_3", "MI1_4",
      "MI1_5", "MI1_6", "DO1_1", "DO1_2", "DO1_3", "DO1_4", "DO1_5", "DO1_6",
      "FR1_1", "FR1_2", "FR1_3", "FR1_4", "FR1_5", "FR1_6", "SA1_1", "SA1_2",
      "SA1_3", "SA1_4", "SA1_5", "SA1_6", "MO2_1", "MO2_2", "MO2_3", "MO2_4",
      "MO2_5", "MO2_6", "DI2_1", "DI2_2", "DI2_3", "DI2_4", "DI2_5", "DI2_6",
      "MI2_1", "MI2_2", "MI2_3", "MI2_4", "MI2_5", "MI2_6", "DO2_1", "DO2_2",
      "DO2_3", "DO2_4", "DO2_5", "DO2_6", "FR2_1", "FR2_2", "FR2_3", "FR2_4",
      "FR2_5", "FR2_6", "SA2_1", "SA2_2", "SA2_3", "SA2_4", "SA2_5", "SA2_6",
      "MO3_1", "MO3_2", "MO3_3", "MO3_4", "MO3_5", "MO3_6", "DI3_1", "DI3_2",
      "DI3_3", "DI3_4", "DI3_5", "DI3_6", "MI3_1", "MI3_2", "MI3_3", "MI3_4",
      "MI3_5", "MI3_6", "DO3_1", "DO3_2", "DO3_3", "DO3_4", "DO3_5", "DO3_6",
      "FR3_1", "FR3_2", "FR3_3", "FR3_4", "FR3_5", "FR3_6", "SA3_1", "SA3_2",
      "SA3_3", "SA3_4", "SA3_5", "SA3_6"};

 public:
  /**
   *  @brief Creates a PlanCsvHelper for the given path
   *  @param path is the directory the csv files are located
   */
  PlanCsvHelper(QString path);

  /**
   *  @brief Creates a PlanCsvHelper
   *
   *  Creates a PlanCsvHelper with a temporary directory for the files. The
   * directory will be removed, once this PlanCsvHelper gets destroyed.
   */
  PlanCsvHelper();

  /**
   *  @brief Create a plan from the files in path
   *  @param [in] parent is the parent QObject of the generated plan
   *  @return The plan generated from the files in path
   *
   *  The generated plan will not contain any scheduling information
   *  If csv files are missing, a nullptr will be returned
   *  If the parent is a nullptr you are responsible for deleting
   */
  Plan* readPlan(QObject* parent = nullptr);

  /**
   *  @brief Write plan to csv files
   *  @param [in] plan is the plan, that will be written
   *  @return True if the plan was written successfully
   *
   *  Writes the plan to csv files.
   *  If the plan contains scheduling information it will also be written, but
   *  not to the groupsExamsResultFile. If csv files already exist in the path,
   *  everything there will be deleted and new csv files will be written.
   */
  bool writePlan(Plan* plan);

  /**
   *  @brief Check if the files required by sp-automatisch exist
   *  @return True, if the files required by sp-automatisch exist
   *
   *  These files are required:
   *   - pruef-intervalle.csv
   *   - pruefungen.csv
   *   - zuege-pruef.csv
   *   - zuege-pruef-pref2.csv
   */
  bool isWritten();

  /**
   *  @brief Check if the plan is scheduled
   *  @return True, if the files that are generated by sp-automatisch exist
   *
   *  These files are checked:
   *   - SPA-ERGEBNIS-PP/SPA-planung-pruef.csv
   *   - SPA-ERGEBNIS-PP/SPA-zuege-pruef.cs
   */
  bool isScheduled();

  /**
   *  @brief Get the path of the directory containing the csv files
   *  @return A QString containing the path
   */
  QString getPath();

  // TODO decide if this should check, that plan is the same plan as in the csv
  // files. Currently it does not
  // TODO decide if this should check, that all modules have been assigned a
  // date
  /**
   *  @brief Add the scheduling information from the files to a plan
   *  @param [in] plan is a pointer to the plan to which the scheduling
   * information will be added
   *  @return True if succeeded
   *
   * Every scheduled module, that is read from the csv file will only be
   * scheduled once in plan. Old schedulings for modules contained in the csv
   * file will be removed.
   */
  bool readSchedule(Plan* plan);

 private:
  /**
   *  @brief Initialize the QFile objects with the correct paths
   */
  void initializeFilePaths();

  /**
   *  @brief Write the pruef-intervalle.csv file
   *  @param [in] plan is the plan, that will be written
   *  @return True if the the file was written successfully
   */
  bool writeExamsIntervalsFile(Plan* plan);

  /**
   *  @brief Write the pruefungen.csv file
   *  @param [in] plan is the plan, that will be written
   *  @return True if the the file was written successfully
   */
  bool writeExamsFile(Plan* plan);

  /**
   *  @brief Write the zuege-pruef.csv file
   *  @param [in] plan is the plan, that will be written
   *  @return True if the the file was written successfully
   */
  bool writeGroupsExamsFile(Plan* plan);

  /**
   *  @brief Write the zuege-pruef-pref2.csv file
   *  @param [in] plan is the plan, that will be written
   *  @return True if the the file was written successfully
   */
  bool writeGroupsExamsPrefFile(Plan* plan);

  /**
   *  @brief Write the SPA-ERGEBNIS-PP/SPA-planung-pruef.csv file
   *  @param [in] plan is the plan, that will be written
   *  @return True if the the file was written successfully
   */
  bool writePlanningExamsResultFile(Plan* plan);

  /**
   *  @brief Read the pruef-intervalle.csv file and add the information to plan
   *  @param plan is the Plan
   *  @return True if the the file was read successfully
   *
   * If reading the file fails, plan has to be considered as invalid and get
   * deleted
   */
  bool readExamsIntervalsFile(Plan* plan);

  /**
   *  @brief Read the pruefungen.csv file and add the information to plan
   *  @param plan is the Plan
   *  @return True if the the file was read successfully
   *
   * If reading the file fails, plan has to be considered as invalid and get
   * deleted
   */
  bool readExamsFile(Plan* plan, bool parseComments = true, bool addMissingGroups = true);

  /**
   *  @brief Read the zuege-pruef.csv file and add the information to plan
   *  @param plan is the Plan
   *  @return True if the the file was read successfully
   *
   * If reading the file fails, plan has to be considered as invalid and get
   * deleted
   */
  bool readGroupsExamsFile(Plan* plan);

  /**
   *  @brief Read the zuege-pruef-pref2.csv file and add the information to
   * plan
   *  @param plan is the Plan
   *  @return True if the the file was read successfully
   *
   * If reading the file fails, plan has to be considered as invalid and get
   * deleted
   */
  bool readGroupsExamsPrefFile(Plan* plan);
};

#endif  // PLANCSVHELPER_H
