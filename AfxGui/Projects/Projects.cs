// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-09-30 by dominik.matrixstorm.com
//
// First changes:
// 2010-09-30 by dominik.matrixstorm.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;


namespace AfxGui.Projects
{

interface IProject
{
    /// <summary>Closes the project.</summary>
    void Close();

    /// <summary>
    /// Requests to create a save state.
    /// </summary>
    /// <returns>A serializeable Object.</returns>
    Object Save();

    /// <summary>
    /// Factory of this project.
    /// </summary>
    IProjectFactory Factory { get; }

    /// <summary>
    ///   The responsible object.<br />
    ///   The object may supply further obligatory
    ///   interfaces for GUI integration.
    /// </summary>
    Object Object { get; }
}

interface IProjectFactory
{
    /// <summary>
    /// Creates a new project.
    /// </summary>
    IProject New();

    /// <summary>
    /// Tells the factory to load from a deserialized object.
    /// </summary>
    /// <param name="data">data object to load from</param>
    /// <returns>null on fail, project otherwise</returns>
    IProject Load(Object data);

    /// <summary>
    /// Factory description.
    /// </summary>
    String Description { get; }

    /// <summary>
    /// Unique factory identifyer.
    /// </summary>
    Guid Id { get; }

    /// <summary>
    /// Factory title.
    /// </summary>
    String Title { get; }
}

interface IProjectMenu
{
    ToolStripDropDown Menu { get; }
}

interface IProjectWizard
{
    void Wizard(IWin32Window ownerWindow);
}

class ProjectManager
{
    public void Close()
    {
        if (null == this.project) return;

        this.project.Close();

        this.project = null;
    }

    public void Load(ICollection<IProjectFactory> factories, String filePath)
    {
        Close();

        AfxData.Project projectData = null;

        if (File.Exists(filePath))
		{
            using (FileStream fs = new FileStream(filePath, FileMode.Open))
            {
                try
                {
                    XmlSerializer serializer = new XmlSerializer(typeof(AfxData.Project));
                    projectData = serializer.Deserialize(fs) as AfxData.Project;
                }
                catch(Exception)
                {
                    projectData = null;
                }
            }
        }

        if(null != projectData)
        {
            foreach (IProjectFactory factory in factories)
            {
                if (projectData.FactoryId == factory.Id)
                {
                    this.project = factory.Load(projectData.Data);
                    break;
                }
            }
        }
    }

    public void New(IProjectFactory factory)
    {
        Close();

        this.project = factory.New();
    }

    /// <summary>
    /// Save the current project to a file.
    /// </summary>
    /// <param name="filePath">Path to save to</param>
    /// <returns>True upon success, false otherwise.</returns>
    public bool Save(String filePath)
    {
        if (null == this.project) return false;

        AfxData.Project projectData = new AfxData.Project();

        projectData.Data = this.project.Save();
        projectData.FactoryId = this.project.Factory.Id;

        if (null == projectData.Data) return false;

        bool bOk = false;

        using (TextWriter writer = new StreamWriter(filePath))
        {
            try
            {
                XmlSerializer serializer = new XmlSerializer(typeof(AfxData.Project));
                serializer.Serialize(writer, projectData);

                bOk = true;
            }
            catch (Exception)
            {
                bOk = false;
            }
        }

        return bOk;
    }

    public IProject Project
    {
        get
        {
            return project;
        }
    }

    IProject project;
}

}