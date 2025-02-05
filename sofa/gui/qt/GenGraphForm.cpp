/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*            (c) 2006-2021 INRIA, USTL, UJF, CNRS, MGH, InSimo                *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#include "GenGraphForm.h"
#include <sofa/simulation/tree/ExportDotVisitor.h>
#include <sofa/helper/system/SetDirectory.h>
#include <sofa/helper/system/FileSystem.h>

#ifdef WIN32
#include <windows.h>
#endif

#include <fstream>

#ifdef SOFA_QT4
#include "q3filedialog.h"
#else
#include "qlistview.h"
#include "qcheckbox.h"
#include "qpushbutton.h"
#include "qlineedit.h"
#include "qfiledialog.h"
#include "qheader.h"
#include "qcombobox.h"
#include "qradiobutton.h"
#endif

namespace sofa
{

namespace gui
{

namespace qt
{

#ifndef SOFA_QT4
typedef QListView Q3ListView;
typedef QListViewItem Q3ListViewItem;
typedef QFileDialog Q3FileDialog;
#endif

GenGraphForm::GenGraphForm()
    : currentTask(NULL), settingFilter(false)
{
    setupUi(this);

    // init preset filters
    {
        std::set<std::string>& filt = presetFilters["Overview"];
        filt.insert("showNodes");
        filt.insert("showObjects");
        //filt.insert("showSlaves");
        filt.insert("showBehaviorModels");
        filt.insert("showCollisionModels");
        filt.insert("showVisualModels");
        //filt.insert("showVisualObjects");
        filt.insert("showMappings");
        //filt.insert("showContext");
        //filt.insert("showEngine");
        //filt.insert("showLoader");
        //filt.insert("showCollisionPipeline");
        //filt.insert("showAnimationLoop");
        filt.insert("showControllers");
        filt.insert("showSolvers");
        filt.insert("showMechanicalStates");
        //filt.insert("showForceFields");
        //filt.insert("showInteractionForceFields");
        //filt.insert("showConstraints");
        //filt.insert("showProjectiveConstraints");
        //filt.insert("showMass");
        filt.insert("showTopology");
        //filt.insert("showTopologyObjects");
        filt.insert("showTopologicalMappings");
        filt.insert("showMechanicalMappings");
        //filt.insert("showOthers");
    }
    {
        std::set<std::string>& filt = presetFilters["Full Graph"];
        filt.insert("showNodes");
        filt.insert("showObjects");
        filt.insert("showSlaves");
        filt.insert("showBehaviorModels");
        filt.insert("showCollisionModels");
        filt.insert("showVisualModels");
        filt.insert("showVisualObjects");
        filt.insert("showMappings");
        filt.insert("showContext");
        filt.insert("showEngine");
        filt.insert("showLoader");
        filt.insert("showCollisionPipeline");
        filt.insert("showAnimationLoop");
        filt.insert("showControllers");
        filt.insert("showSolvers");
        filt.insert("showMechanicalStates");
        filt.insert("showForceFields");
        filt.insert("showInteractionForceFields");
        filt.insert("showConstraints");
        filt.insert("showProjectiveConstraints");
        filt.insert("showMass");
        filt.insert("showTopology");
        filt.insert("showTopologyObjects");
        filt.insert("showTopologicalMappings");
        filt.insert("showMechanicalMappings");
        filt.insert("showOthers");
    }
    {
        std::set<std::string>& filt = presetFilters["All Objects"];
        filt = presetFilters["Full Graph"];
        filt.erase("showNodes");
    }
    {
        std::set<std::string>& filt = presetFilters["All Nodes"];
        filt.insert("showNodes");
    }
    {
        std::set<std::string>& filt = presetFilters["Full Mechanical Graph"];
        filt = presetFilters["Full Graph"];
        filt.erase("showCollisionModels");
        filt.erase("showVisualModels");
        filt.erase("showVisualObjects");
        filt.erase("showCollisionPipeline");
        filt.erase("showTopologyObjects");
        filt.erase("showControllers");
        filt.erase("showEngine");
        filt.erase("showLoader");
        filt.erase("showSlaves");
        filt.erase("showOthers");
    }
    {
        std::set<std::string>& filt = presetFilters["Full Mechanical Objects"];
        filt = presetFilters["Full Mechanical Graph"];
        filt.erase("showNodes");
    }
    {
        std::set<std::string>& filt = presetFilters["Mechanical Graph"];
        filt = presetFilters["Full Mechanical Graph"];
        filt.erase("showAnimationLoop");
        filt.erase("showContext");
        filt.erase("showTopology");
        filt.erase("showTopologicalMappings");
        filt.erase("showMappings");
    }
    {
        std::set<std::string>& filt = presetFilters["Mechanical Objects"];
        filt = presetFilters["Mechanical Graph"];
        filt.erase("showNodes");
    }
    {
        std::set<std::string>& filt = presetFilters["Visual Graph"];
        filt.insert("showNodes");
        filt.insert("showObjects");
        filt.insert("showVisualModels");
        filt.insert("showVisualObjects");
    }
    {
        std::set<std::string>& filt = presetFilters["Visual Objects"];
        filt = presetFilters["Visual Graph"];
        filt.erase("showNodes");
    }
    {
        std::set<std::string>& filt = presetFilters["Collision Graph"];
        filt.insert("showNodes");
        filt.insert("showObjects");
        filt.insert("showCollisionModels");
        filt.insert("showCollisionPipeline");
    }
    {
        std::set<std::string>& filt = presetFilters["Collision Objects"];
        filt = presetFilters["Collision Graph"];
        filt.erase("showNodes");
    }
    {
        std::set<std::string>& filt = presetFilters["Collision Response Graph"];
        filt.insert("showNodes");
        filt.insert("showObjects");
        //filt.insert("showBehaviorModels");
        filt.insert("showCollisionModels");
        //filt.insert("showVisualModels");
        //filt.insert("showVisualObjects");
        //filt.insert("showMappings");
        //filt.insert("showContext");
        //filt.insert("showEngine");
        //filt.insert("showLoader");
        //filt.insert("showCollisionPipeline");
        //filt.insert("showAnimationLoop");
        //filt.insert("showControllers");
        filt.insert("showSolvers");
        filt.insert("showMechanicalStates");
        //filt.insert("showForceFields");
        filt.insert("showInteractionForceFields");
        filt.insert("showConstraints");
        //filt.insert("showProjectiveConstraints");
        //filt.insert("showMass");
        //filt.insert("showTopology");
        //filt.insert("showTopologyObjects");
        //filt.insert("showTopologicalMappings");
        filt.insert("showMechanicalMappings");
    }
    {
        std::set<std::string>& filt = presetFilters["Collision Response Objects"];
        filt = presetFilters["Collision Response Graph"];
        filt.erase("showNodes");
    }

    setFilter();

    // signals and slots connections
    connect(browseButton, SIGNAL(clicked()), this, SLOT(doBrowse()));
    connect(browseGraphvizButton, SIGNAL(clicked()), this, SLOT(doBrowseGraphviz()));
    connect(exportButton, SIGNAL(clicked()), this, SLOT(doExport()));
    connect(displayButton, SIGNAL(clicked()), this, SLOT(doDisplay()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(doClose()));
    connect(filename, SIGNAL(textChanged(const QString&)), this, SLOT(change()));
    connect(presetFilter, SIGNAL(activated(const QString&)), this, SLOT(setFilter()));
    connect(showNodes, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showObjects, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showSlaves, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showBehaviorModels, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showCollisionModels, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showVisualModels, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showVisualObjects, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showMappings, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showContext, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showEngine, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showLoader, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showCollisionPipeline, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showAnimationLoop, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showControllers, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showSolvers, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showMechanicalStates, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showForceFields, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showInteractionForceFields, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showConstraints, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showProjectiveConstraints, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showMass, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showTopology, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showTopologyObjects, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showTopologicalMappings, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showMechanicalMappings, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
    connect(showOthers, SIGNAL(toggled(bool)), this, SLOT(changeFilter()));
#ifdef WIN32
    std::string pfdir = "C:\\Program Files (x86)";
    if (!sofa::helper::system::FileSystem::exists(pfdir))
    {
        pfdir = "C:\\Program Files (x86)";
    }
    std::vector<std::string> dirs;
    sofa::helper::system::FileSystem::listDirectory(pfdir, dirs);
    for(const std::string& s : dirs)
    {
        if (s.substr(0, 8) == "Graphviz")
        {
            std::string gdir = pfdir + std::string("\\") + s + std::string("\\bin");
            if (sofa::helper::system::FileSystem::exists(gdir + std::string("\\dot.exe")))
            {
                graphvizdir->setText(gdir.c_str());
                break;
            }
            //graphvizdir->setText("C:\\Program Files (x86)\\Graphviz2.38\\bin");
        }
    }
#endif
}

void GenGraphForm::change()
{
    //exported = false;
    displayButton->setEnabled(false);
    if (exportButton->text() != "&Kill")
        exportButton->setEnabled(filename->text()!=QString(""));
}

void GenGraphForm::changeFilter()
{
    displayButton->setEnabled(false);
    if (settingFilter) return;
    std::set<std::string> filt = getCurrentFilter();
    for (std::map<std::string,std::set<std::string> >::const_iterator it = presetFilters.begin(); it != presetFilters.end(); ++it)
    {
        if (it->second == filt)
        {
            presetFilter->setCurrentText(it->first.c_str());
            return;
        }
    }
    presetFilter->setCurrentText("");
}

QString removeFileExt(const QString &s)
{
    int ext = s.findRev('.');
    if (ext==-1) return s;
    if (s.find('/', ext+1)!=-1 || s.find('\\', ext+1)!=-1)
        return s;
    return s.left(ext);
}

void GenGraphForm::doBrowse()
{
    QString s = Q3FileDialog::getSaveFileName(
            filename->text(),
            "DOT Graph (*.dot)",
            this,
            "export graph file dialog",
            "Choose a file to save" );
    if (s.length()>0)
    {
        if (removeFileExt(s)==s) // no extension specified
            s += ".dot";
        filename->setText(s);
        change();
    }
}

void GenGraphForm::doBrowseGraphviz()
{
    QString s = Q3FileDialog::getExistingDirectory(
        graphvizdir->text(),
        this,
        "Graphviz bin directory dialog",
        "Choose the directory where Graphviz executables are installed");
    if (s.length()>0)
    {
        graphvizdir->setText(s);
    }
}

void GenGraphForm::doExport()
{
    if (!tasks.empty())
    {
        killAllTasks();
        return;
    }
    if (filename->text()==QString("")) return;

    QString dotfile = filename->text();

    if (!sofa::helper::system::SetDirectory::IsAbsolute(dotfile.ascii()))
    {
        dotfile = QString(sofa::helper::system::SetDirectory::GetRelativeFromDir(dotfile.ascii(), sofa::helper::system::SetDirectory::GetCurrentDir().c_str()).c_str());
    }

    QString basefile = removeFileExt(dotfile);

    if (basefile==dotfile) // no extension specified
        dotfile += ".dot";

    std::ofstream fdot;
    fdot.open(dotfile, std::ofstream::out | std::ofstream::trunc);
    if (!fdot.is_open())
    {
        qFatal("Output to %s failed.\n",(const char*)dotfile);
        return;
    }
    std::string layout = "dot";
    std::string rankdir;
    if (layoutDirV->isOn() || layoutDirVG->isOn())
    {
        rankdir = "TB";
    }
    else if (layoutDirH->isOn() || layoutDirHG->isOn())
    {
        rankdir = "LR";
    }
    else if (layoutSpring->isOn())
    {
        layout = "neato";
    }
    else if (layoutRadial->isOn())
    {
        layout = "twopi";
    }

    {
        sofa::simulation::tree::ExportDotVisitor act(sofa::core::ExecParams::defaultInstance() /* PARAMS FIRST */, &fdot);
        act.graphAttrs["layout"] = layout;
        if (!rankdir.empty())
        {
            act.graphAttrs["rankdir"] = rankdir;
        }
        act.showNode = this->showNodes->isOn();
        act.showObject = this->showObjects->isOn();
        act.showSlaves = this->showSlaves->isOn();
        act.showBehaviorModel = this->showBehaviorModels->isOn();
        act.showController = this->showControllers->isOn();
        act.showCollisionModel = this->showCollisionModels->isOn();
        act.showVisualModel = this->showVisualModels->isOn();
        act.showVisualObject = this->showVisualObjects->isOn();
        act.showMapping = this->showMappings->isOn();
        act.showContext = this->showContext->isOn();
        act.showEngine = this->showEngine->isOn();
        act.showLoader = this->showLoader->isOn();
        act.showCollisionPipeline = this->showCollisionPipeline->isOn();
        act.showAnimationLoop = this->showAnimationLoop->isOn();
        act.showSolver = this->showSolvers->isOn();
        act.showMechanicalState = this->showMechanicalStates->isOn();
        act.showForceField = this->showForceFields->isOn();
        act.showInteractionForceField = this->showInteractionForceFields->isOn();
        act.showConstraint = this->showConstraints->isOn();
        act.showProjectiveConstraint = this->showProjectiveConstraints->isOn();
        act.showMass = this->showMass->isOn();
        act.showTopology = this->showTopology->isOn();
        act.showTopologyObject = this->showTopologyObjects->isOn();
        act.showTopologicalMapping = this->showTopologicalMappings->isOn();
        act.showMechanicalMapping = this->showMechanicalMappings->isOn();
        act.showOthers = this->showOthers->isOn();
        act.labelNodeName = this->labelNodeName->isOn();
        act.labelNodeClass = this->labelNodeClass->isOn();
        act.labelObjectName = this->labelObjectName->isOn();
        act.labelObjectClass = this->labelObjectClass->isOn();
        act.showSolverGroups = layoutDirVG->isOn() || layoutDirHG->isOn();
        {
            std::istringstream i((const char*)this->includeNames->text());
            i >> act.includeNames;
        }
        {
            std::istringstream i((const char*)this->excludeNames->text());
            i >> act.excludeNames;
        }
        {
            std::istringstream i((const char*)this->inputLinks->text());
            i >> act.inputLinks;
        }
        {
            std::istringstream i((const char*)this->outputLinks->text());
            i >> act.outputLinks;
        }
        graph->executeVisitor(&act);
    }
    fdot.close();

    QStringList argv0;
    QString gvdir = graphvizdir->text();
    if (!gvdir.isEmpty())
    {
#ifdef WIN32
        QString executable = gvdir + QString("\\") + QString(layout.c_str()) + QString(".exe");
#else
        QString executable = gvdir + QString("/") + QString(layout.c_str());
#endif
        argv0 << executable;
    }
    else
    {
        argv0 << layout.c_str();
    }
    bool exp = false;

    exportedFile = dotfile;

    if (genSVG->isOn())
    {
        QStringList argv = argv0;
        argv << "-Tsvg" << "-o" << basefile+".svg";
        argv << dotfile;
        addTask(argv);
        if (!exp)
        {
            exp = true;
            exportedFile = basefile+".svg";
        }
    }

    if (genPDF->isOn())
    {
        QStringList argv = argv0;
        argv << "-Tpdf" << "-o" << basefile+".pdf";
        argv << dotfile;
        addTask(argv);
        if (!exp)
        {
            exp = true;
            exportedFile = basefile+".pdf";
        }
    }

    if (genPNG->isOn())
    {
        QStringList argv = argv0;
        argv << "-Tpng" << "-o" << basefile+".png";
        argv << dotfile;
        addTask(argv);
        if (!exp)
        {
            exp = true;
            exportedFile = basefile+".png";
        }
    }

    if (genPS->isOn())
    {
        QStringList argv = argv0;
        argv << "-Tps" << "-o" << basefile+".ps";
        argv << dotfile;
        addTask(argv);
        if (!exp)
        {
            exp = true;
            exportedFile = basefile+".ps";
        }
    }
    //exported = true;
    //displayButton->setEnabled(true);
}

void GenGraphForm::doDisplay()
{
    if (exportedFile==QString("")) return;

    std::cout << "OPEN " << (const char*)exportedFile << std::endl;

#ifdef WIN32
    ShellExecuteA(NULL, "open", exportedFile, NULL, NULL, SW_SHOWNORMAL);
#else
    QStringList argv;
    argv << "xdg-open" << exportedFile;
#ifdef SOFA_QT4
    QString program = argv.front();
    argv.pop_front();
    //QProcess::startDetached(program, argv); //QString("start \"\"\"")+exportedFile+QString("\"\"\""));
    QProcess::startDetached(program, argv); //QString("start \"\"\"")+exportedFile+QString("\"\"\""));
#else
    QProcess* p = new QProcess(argv, this);
    p->setCommunication(0);
    connect(p,SIGNAL(processExited()),p,SLOT(deleteLater()));
    p->start();
#endif
#endif
}

void GenGraphForm::doClose()
{
    killAllTasks();
    this->close();
}

void GenGraphForm::setScene(sofa::simulation::Node* scene)
{
    graph = scene;
}

void GenGraphForm::killAllTasks()
{
    exportButton->setText("&Export");
    if (currentTask)
    {
        currentTask->kill();
        delete currentTask;
        currentTask = NULL;
    }
    tasks.clear();
}

void GenGraphForm::addTask(QStringList argv)
{
    tasks.push_back(argv);
    if (currentTask == NULL)
        runTask();
}

void GenGraphForm::taskFinished()
{
    std::cout << "TASK FINISHED" << std::endl;
    if (currentTask == NULL) return;
//#ifdef SOFA_QT4
//	if (currentTask->state() != QProcess::NotRunning) return;
//#else
//	if (currentTask->isRunning()) return;
//#endif
    delete currentTask;
    currentTask = NULL;
    if (tasks.empty())
    {
        displayButton->setEnabled(true);
        exportButton->setText("&Export");
    }
    else
    {
        runTask();
    }
}

void GenGraphForm::runTask()
{
    QStringList argv = tasks.front();
    tasks.pop_front();
    exportButton->setText("&Kill");
    QString cmd = argv.join(QString(" "));
    std::cout << "STARTING TASK " << (const char*)cmd << std::endl;
#ifdef SOFA_QT4
    QProcess* p = new QProcess(this);
    QString program = argv.front();
    argv.pop_front();
    p->setReadChannelMode(QProcess::ForwardedChannels);
    connect(p,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(taskFinished()));
    p->start(program, argv);
#else
    QProcess* p = new QProcess(argv, this);
    p->setCommunication(0);
    connect(p,SIGNAL(processExited()),this,SLOT(taskFinished()));
    p->start();
#endif
    currentTask = p;
}

void GenGraphForm::setFilter()
{
    QString fname = presetFilter->currentText();
    if (fname == "") return;
    std::string name = (const char*)fname;
    if (presetFilters.count(name) > 0)
    {
        const std::set<std::string> & filt = presetFilters[name];
        settingFilter = true;
        this->showNodes->setChecked(filt.find("showNodes")!=filt.end());
        this->showObjects->setChecked(filt.find("showObjects")!=filt.end());
        this->showSlaves->setChecked(filt.find("showSlaves")!=filt.end());
        this->showBehaviorModels->setChecked(filt.find("showBehaviorModels")!=filt.end());
        this->showControllers->setChecked(filt.find("showControllers")!=filt.end());
        this->showCollisionModels->setChecked(filt.find("showCollisionModels")!=filt.end());
        this->showVisualModels->setChecked(filt.find("showVisualModels")!=filt.end());
        this->showVisualObjects->setChecked(filt.find("showVisualObjects")!=filt.end());
        this->showMappings->setChecked(filt.find("showMappings")!=filt.end());
        this->showContext->setChecked(filt.find("showContext")!=filt.end());
        this->showEngine->setChecked(filt.find("showEngine")!=filt.end());
        this->showLoader->setChecked(filt.find("showLoader")!=filt.end());
        this->showCollisionPipeline->setChecked(filt.find("showCollisionPipeline")!=filt.end());
        this->showAnimationLoop->setChecked(filt.find("showAnimationLoop")!=filt.end());
        this->showSolvers->setChecked(filt.find("showSolvers")!=filt.end());
        this->showMechanicalStates->setChecked(filt.find("showMechanicalStates")!=filt.end());
        this->showForceFields->setChecked(filt.find("showForceFields")!=filt.end());
        this->showInteractionForceFields->setChecked(filt.find("showInteractionForceFields")!=filt.end());
        this->showConstraints->setChecked(filt.find("showConstraints")!=filt.end());
        this->showProjectiveConstraints->setChecked(filt.find("showProjectiveConstraints")!=filt.end());
        this->showMass->setChecked(filt.find("showMass")!=filt.end());
        this->showTopology->setChecked(filt.find("showTopology")!=filt.end());
        this->showTopologyObjects->setChecked(filt.find("showTopologyObjects")!=filt.end());
        this->showTopologicalMappings->setChecked(filt.find("showTopologicalMappings")!=filt.end());
        this->showMechanicalMappings->setChecked(filt.find("showMechanicalMappings")!=filt.end());
        this->showOthers->setChecked(filt.find("showOthers")!=filt.end());
        settingFilter = false;
        displayButton->setEnabled(false);
        presetFilter->setCurrentText(fname);
    }
    else
    {
        //if (QMessage::question(this,"Graph Preset Filters",...)
        presetFilters[name] = getCurrentFilter();
    }
}

std::set<std::string> GenGraphForm::getCurrentFilter()
{
    std::set<std::string> filt;
    if (this->showNodes->isOn()) filt.insert("showNodes");
    if (this->showObjects->isOn()) filt.insert("showObjects");
    if (this->showSlaves->isOn()) filt.insert("showSlaves");
    if (this->showBehaviorModels->isOn()) filt.insert("showBehaviorModels");
    if (this->showControllers->isOn()) filt.insert("showControllers");
    if (this->showCollisionModels->isOn()) filt.insert("showCollisionModels");
    if (this->showVisualModels->isOn()) filt.insert("showVisualModels");
    if (this->showVisualObjects->isOn()) filt.insert("showVisualObjects");
    if (this->showMappings->isOn()) filt.insert("showMappings");
    if (this->showContext->isOn()) filt.insert("showContext");
    if (this->showEngine->isOn()) filt.insert("showEngine");
    if (this->showLoader->isOn()) filt.insert("showLoader");
    if (this->showCollisionPipeline->isOn()) filt.insert("showCollisionPipeline");
    if (this->showAnimationLoop->isOn()) filt.insert("showAnimationLoop");
    if (this->showSolvers->isOn()) filt.insert("showSolvers");
    if (this->showMechanicalStates->isOn()) filt.insert("showMechanicalStates");
    if (this->showForceFields->isOn()) filt.insert("showForceFields");
    if (this->showInteractionForceFields->isOn()) filt.insert("showInteractionForceFields");
    if (this->showConstraints->isOn()) filt.insert("showConstraints");
    if (this->showProjectiveConstraints->isOn()) filt.insert("showProjectiveConstraints");
    if (this->showMass->isOn()) filt.insert("showMass");
    if (this->showTopology->isOn()) filt.insert("showTopology");
    if (this->showTopologyObjects->isOn()) filt.insert("showTopologyObjects");
    if (this->showTopologicalMappings->isOn()) filt.insert("showTopologicalMappings");
    if (this->showMechanicalMappings->isOn()) filt.insert("showMechanicalMappings");
    if (this->showOthers->isOn()) filt.insert("showOthers");
    return filt;
}

} // namespace qt

} // namespace gui

} // namespace sofa
