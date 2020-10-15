#ifndef PLANCSVHELPER_TEST_CPP
#define PLANCSVHELPER_TEST_CPP

#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <plancsvhelper.h>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTemporaryDir>
#include <QTextStream>
#include "plan.h"
#include "qthelper.cpp"
#include "testdatahelper.h"

using namespace testing;

TEST(planCsvHelperTests, constructorSetsPath) {
  QString path = QDir::currentPath();
  PlanCsvHelper helper(path);
  EXPECT_EQ(helper.getPath(), path);
}

TEST(planCsvHelperTests, constructorCreatesTemporaryDirectory) {
  PlanCsvHelper helper;
  EXPECT_NE(helper.getPath(), "");
  EXPECT_TRUE(QDir().exists(helper.getPath()))
      << "PlanCsvHelper did not create a temporary directory at "
      << helper.getPath().constData() << ".";
}

TEST(planCsvHelperTests, temporaryDirectoryGetsRemovedAfterDestruction) {
  PlanCsvHelper* helper = new PlanCsvHelper();
  QString path = helper->getPath();
  delete helper;
  EXPECT_FALSE(QDir().exists(path))
      << "PlanCsvHelper did not remove temporary directory at "
      << helper->getPath().constData() << " after destruction.";
}

TEST(planCsvHelperTests, normalDirectoryDoesNotGetRemovedAfterDestruction) {
  QTemporaryDir tempdir;
  PlanCsvHelper* helper = new PlanCsvHelper(tempdir.path());
  QString path = helper->getPath();
  delete helper;
  EXPECT_TRUE(QDir().exists(path))
      << "PlanCsvHelper did remove directory at "
      << helper->getPath().constData() << " after destruction.";
}

TEST(planCsvHelperTests, isScheduledDetectsNoResultDirectory) {
  QTemporaryDir noResultDirectory;
  PlanCsvHelper helper(noResultDirectory.path());
  EXPECT_FALSE(helper.isScheduled()) << "Detected as scheduled, but it is not";
}

TEST(planCsvHelperTests, isScheduledDetectsEmptyResultDirectory) {
  QTemporaryDir emptyResultDirectory;
  QDir().mkdir(emptyResultDirectory.path() + "/SPA-ERGEBNIS-PP/");
  PlanCsvHelper helper(emptyResultDirectory.path());
  EXPECT_FALSE(helper.isScheduled()) << "Detected as scheduled, but it is not";
}

TEST(planCsvHelperTests, isScheduledDetectsPartialEmptyResultDirectory) {
  QTemporaryDir partialResultDirectory1;
  QDir().mkdir(partialResultDirectory1.path() + "/SPA-ERGEBNIS-PP/");
  QFile(partialResultDirectory1.path().append(
            "/SPA-ERGEBNIS-PP/SPA-planung-pruef.csv"))
      .open(QFile::ReadWrite);
  PlanCsvHelper helper1(partialResultDirectory1.path());
  EXPECT_FALSE(helper1.isScheduled()) << "Detected as scheduled, but it is not";

  QTemporaryDir partialResultDirectory2;
  QDir().mkdir(partialResultDirectory2.path() + "/SPA-ERGEBNIS-PP/");
  QFile(partialResultDirectory2.path().append(
            "/SPA-ERGEBNIS-PP/SPA-zuege-pruef.csv"))
      .open(QFile::ReadWrite);
  PlanCsvHelper helper2(partialResultDirectory1.path());
  EXPECT_FALSE(helper2.isScheduled()) << "Detected as scheduled, but it is not";
}

TEST(planCsvHelperTests, isScheduledDetectsFullResultDirectory) {
  QTemporaryDir fullResultDirectory;
  QDir().mkdir(fullResultDirectory.path() + "/SPA-ERGEBNIS-PP/");
  QFile(fullResultDirectory.path().append(
            "/SPA-ERGEBNIS-PP/SPA-planung-pruef.csv"))
      .open(QFile::ReadWrite);
  QFile(fullResultDirectory.path() + "/SPA-ERGEBNIS-PP/SPA-zuege-pruef.csv")
      .open(QFile::ReadWrite);
  PlanCsvHelper helper(fullResultDirectory.path());
  EXPECT_TRUE(helper.isScheduled()) << "Did not detect full result directory";
}

TEST(planCsvHelperTests, isWrittenDetectsEmptyDirectory) {
  QTemporaryDir emptyResultDirectory;
  PlanCsvHelper helper(emptyResultDirectory.path());
  EXPECT_FALSE(helper.isWritten()) << "Detected as written, but it is not";
}

TEST(planCsvHelperTests, isWrittenDetectsPartialEmptyDirectory) {
  QTemporaryDir directory1;
  QTemporaryDir directory2;
  QFile(directory1.path() + "/pruef-intervalle.csv").open(QFile::ReadWrite);
  QFile(directory1.path() + "/pruefungen.csv").open(QFile::ReadWrite);
  QFile(directory2.path() + "/zuege-pruef.csv").open(QFile::ReadWrite);
  QFile(directory2.path() + "/zuege-pruef-pref2.csv").open(QFile::ReadWrite);
  PlanCsvHelper helper1(directory1.path());
  PlanCsvHelper helper2(directory2.path());
  EXPECT_FALSE(helper1.isWritten()) << "Detected as written, but it is not";
  EXPECT_FALSE(helper2.isWritten()) << "Detected as written, but it is not";
}

TEST(planCsvHelperTests, isWrittenDetectsFiles) {
  QTemporaryDir directory;
  QFile(directory.path() + "/pruef-intervalle.csv").open(QFile::ReadWrite);
  QFile(directory.path() + "/pruefungen.csv").open(QFile::ReadWrite);
  QFile(directory.path() + "/zuege-pruef.csv").open(QFile::ReadWrite);
  QFile(directory.path() + "/zuege-pruef-pref2.csv").open(QFile::ReadWrite);
  PlanCsvHelper helper(directory.path());
  EXPECT_TRUE(helper.isWritten());
}

TEST(planCsvHelperTests, writePlanCreatesFiles) {
  QSharedPointer<Plan> plan = getValidPlan();

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  EXPECT_TRUE(helper.writePlan(plan));
  // helper.isWritten is already tested.
  EXPECT_TRUE(helper.isWritten()) << "Plan is not written after check";
}

TEST(planCsvHelperTests, writePlanReturnsFalseWithNullptr) {
  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  EXPECT_FALSE(helper.writePlan(nullptr));
}

TEST(planCsvHelperTests, writePlanDetectsMissingDirectory) {
  QSharedPointer<Plan> plan = getValidPlan();

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path() + "/doesnotexist/");
  EXPECT_FALSE(helper.writePlan(plan))
      << "Write plan succeeded, but the target directory does not exist";
  EXPECT_FALSE(helper.isWritten()) << "Write plan created directory";
}

TEST(planCsvHelperTests, getPathWorks) {
  QString path = QDir::currentPath();
  PlanCsvHelper helper(path);
  EXPECT_NE(helper.getPath(), "");
  EXPECT_EQ(helper.getPath(), path);
}

TEST(planCsvHelperTests, readPlanReturnsNullptrOnFailure) {
  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  EXPECT_EQ(helper.readPlan(), QSharedPointer<Plan>(nullptr));
}

TEST(planCsvHelperTests, readPlanReturnsNullptrOnEmptyFiles) {
  QTemporaryDir directory;
  QFile(directory.path() + "/pruef-intervalle.csv").open(QFile::ReadWrite);
  QFile(directory.path() + "/pruefungen.csv").open(QFile::ReadWrite);
  QFile(directory.path() + "/zuege-pruef.csv").open(QFile::ReadWrite);
  QFile(directory.path() + "/zuege-pruef-pref2.csv").open(QFile::ReadWrite);
  PlanCsvHelper helper(directory.path());
  EXPECT_EQ(helper.readPlan(), QSharedPointer<Plan>(nullptr));
}

TEST(planCsvHelperTests, readPlanReadsWrittenPlan) {
  QSharedPointer<Plan> plan = getValidPlan();

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  helper.writePlan(plan);
  QSharedPointer<Plan> readPlan = helper.readPlan();
  ASSERT_NE(readPlan, QSharedPointer<Plan>(nullptr));
}

TEST(planCsvHelperTests, readPlanReadsWrittenPlanCorrect) {
  QSharedPointer<Plan> plan = getValidPlan();

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  helper.writePlan(plan);
  QSharedPointer<Plan> readPlan = helper.readPlan();
  ASSERT_NE(readPlan, nullptr);

  // Check that no module got lost
  for (Module* module : plan->modules) {
    // EIT modules will not be written, so they will get lost
    if (module->getOrigin() != "EIT") {
      bool found = false;
      for (Module* readModule : readPlan->modules) {
        if (module->name == readModule->name) {
          found = true;
          break;
        }
      }
      ASSERT_TRUE(found) << "The read plan does not contain all modules from "
                            "the original plan. Missing module: "
                         << module->name.constData();
    }
  }
}

TEST(planCsvHelperTests, readScheduleDetectsMissingFiles) {
  QSharedPointer<Plan> plan = getValidPlan();

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  ASSERT_FALSE(helper.readSchedule(plan));
}

TEST(planCsvHelperTests, readScheduleDetectsEmptyFiles) {
  QSharedPointer<Plan> plan = getValidPlan();

  QTemporaryDir directory;
  QDir().mkdir(directory.path() + "/SPA-ERGEBNIS-PP/");
  QFile(directory.path().append("/SPA-ERGEBNIS-PP/SPA-planung-pruef.csv"))
      .open(QFile::ReadWrite);
  QFile(directory.path() + "/SPA-ERGEBNIS-PP/SPA-zuege-pruef.csv")
      .open(QFile::ReadWrite);

  PlanCsvHelper helper(directory.path());
  ASSERT_FALSE(helper.readSchedule(plan));
}

TEST(planCsvHelperTests, readScheduleWorksWithReadPlan) {
  QTemporaryDir directory;
  prepareScheduledDirectory(directory.path());

  PlanCsvHelper helper(directory.path());
  QSharedPointer<Plan> plan = helper.readPlan();
  ASSERT_NE(plan, QSharedPointer<Plan>(nullptr));
  ASSERT_TRUE(helper.readSchedule(plan));
}

TEST(planCsvHelperTests, readScheduleWorksAndAlsoReadsCorrectSchedule) {
  QTemporaryDir directory;
  prepareScheduledDirectory(directory.path());

  PlanCsvHelper helper(directory.path());
  QSharedPointer<Plan> plan = helper.readPlan();
  ASSERT_NE(plan, QSharedPointer<Plan>(nullptr));
  ASSERT_TRUE(helper.readSchedule(plan));

  // Assert that the tested modules can be accessed
  ASSERT_GE(plan->weeks.size(), 2);
  ASSERT_GE(plan->weeks[1]->getDays().size(), 3);
  ASSERT_GE(plan->weeks[1]->getDays()[2]->getTimeslots().size(), 5);
  ASSERT_GE(plan->weeks[1]->getDays()[1]->getTimeslots().size(), 5);
  ASSERT_GE(
      plan->weeks[1]->getDays()[2]->getTimeslots()[4]->getModules().size(), 1);
  ASSERT_GE(
      plan->weeks[1]->getDays()[1]->getTimeslots()[2]->getModules().size(), 1);
  // Check two of the modules from the file
  ASSERT_EQ(plan->weeks[1]
                ->getDays()[2]
                ->getTimeslots()[4]
                ->getModules()[0]
                ->getNumber(),
            "30.2342");
  ASSERT_EQ(plan->weeks[1]
                ->getDays()[1]
                ->getTimeslots()[2]
                ->getModules()[0]
                ->getNumber(),
            "30.2476");
}

TEST(planCsvHelperTests, readScheduleRemovesOldScheduledModules) {
  QTemporaryDir directory;
  prepareScheduledDirectory(directory.path());

  PlanCsvHelper helper(directory.path());
  QSharedPointer<Plan> plan = helper.readPlan();
  ASSERT_NE(plan, QSharedPointer<Plan>(nullptr));
  // Preschedule a module twice, so at least one of them gets removed
  ASSERT_GE(plan->weeks.size(), 1);
  ASSERT_GE(plan->weeks[0]->getDays().size(), 1);
  ASSERT_GE(plan->weeks[0]->getDays()[0]->getTimeslots().size(), 2);
  ASSERT_GE(plan->modules.size(), 1);
  plan->weeks[0]->getDays()[0]->getTimeslots()[0]->addModule(plan->modules[0]);
  plan->weeks[0]->getDays()[0]->getTimeslots()[1]->addModule(plan->modules[0]);
  ASSERT_TRUE(helper.readSchedule(plan));

  EXPECT_FALSE(
      plan->weeks[0]->getDays()[0]->getTimeslots()[0]->getModules().contains(
          plan->modules[0]) &&
      plan->weeks[0]->getDays()[0]->getTimeslots()[1]->getModules().contains(
          plan->modules[0]));
}

TEST(planCsvHelperTests, writePlanWritesProbablyCorrectScheduleFile) {
  QSharedPointer<Plan> plan = getValidPlan();
  ASSERT_GE(plan->weeks.size(), 1);
  ASSERT_GE(plan->weeks[0]->getDays().size(), 1);
  ASSERT_GE(plan->weeks[0]->getDays()[0]->getTimeslots().size(), 2);
  ASSERT_GE(plan->modules.size(), 3);
  plan->weeks[0]->getDays()[0]->getTimeslots()[0]->addModule(plan->modules[0]);
  plan->weeks[0]->getDays()[0]->getTimeslots()[1]->addModule(plan->modules[1]);

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  EXPECT_TRUE(helper.writePlan(plan));
  QFile resultFile(
      directory.path().append("/SPA-ERGEBNIS-PP/SPA-planung-pruef.csv"));
  ASSERT_TRUE(resultFile.open(QFile::ReadOnly));
  QTextStream in(&resultFile);
  int lineCount = 0;
  while (!in.atEnd()) {
    in.readLine();
    lineCount++;
  }
  EXPECT_EQ(lineCount, 3);
}

TEST(planCsvHelperTests, writingAndReadingPlanPreservesModuleExamType) {
  QSharedPointer<Plan> plan = getValidPlan();
  QString firstExamType = plan->getModules()[0]->getExamType();
  QString newFirstExamType = firstExamType == "K" ? "P" : "K";
  plan->getModules()[0]->setExamType(newFirstExamType);
  plan->getModules()[1]->setExamType("K");
  plan->getModules()[2]->setExamType("P");

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  helper.writePlan(plan);
  QSharedPointer<Plan> readPlan = helper.readPlan();
  ASSERT_NE(readPlan, nullptr);
  // TODO maybe do not trust that the module order will be the same
  ASSERT_EQ(readPlan->getModules()[0]->getExamType(), newFirstExamType);
  ASSERT_EQ(readPlan->getModules()[1]->getExamType(), "K");
  ASSERT_EQ(readPlan->getModules()[2]->getExamType(), "P");

  // TODO assert that the results file also contains the correct values
}

TEST(planCsvHelperTests, writingAndReadingPlanPreservesModuleExamDuration) {
  QSharedPointer<Plan> plan = getValidPlan();
  unsigned int firstExamDuration = plan->getModules()[0]->getExamDuration();
  firstExamDuration++;
  plan->getModules()[0]->setExamDuration(firstExamDuration);
  plan->getModules()[1]->setExamDuration(4);
  plan->getModules()[2]->setExamDuration(3);

  QTemporaryDir directory;
  PlanCsvHelper helper(directory.path());
  helper.writePlan(plan);
  QSharedPointer<Plan> readPlan = helper.readPlan();
  ASSERT_NE(readPlan, nullptr);
  // TODO maybe do not trust that the module order will be the same
  ASSERT_EQ(readPlan->getModules()[0]->getExamDuration(), firstExamDuration);
  ASSERT_EQ(readPlan->getModules()[1]->getExamDuration(), 4);
  ASSERT_EQ(readPlan->getModules()[2]->getExamDuration(), 3);
}

#endif  // TEST_CPP
