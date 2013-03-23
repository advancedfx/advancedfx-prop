// Copyright (c) by advancedfx.org
//
// Last changes:
// 2013-03-18 by dominik.matrixstorm.com
//
// First changes:
// 2013-03-18 by dominik.matrixstorm.com

// THIS IS UTTER SHIT AND NOT THOUGHT THROUGH, IGNORE IT FOR NOW!

// - Objects with properties
// - Connect / Disconnect Client
// - Sync to Client on Connect (Create Object and Sync Properties)
// - Destroy in Client when object is disposed
// - Sync Propeties when Changed to client
/*
using System;
using System.Collections.Generic;
using System.Threading;

namespace Afx {

class SyncedObjectHost
{
    public Int32 AddObject(SyncedObject syncedObject)
    {
        Int32 lastValue = -1;
        LinkedListNode<HostedObject> node;

        for (node = m_HostedObjects.First; node != null; node = node.Next)
        {
            Int32 curValue = node.Value.Id;

            if (2 <= curValue - lastValue)
            {
                break;
            }

            lastValue = curValue;
        }

        Int32 newValue = lastValue + 1;

        if (null != node)
        {
            m_HostedObjects.AddBefore(node,
                new HostedObject(newValue, syncedObject)
            );
        }
        else
        {
            m_HostedObjects.AddLast(
                new HostedObject(newValue, syncedObject)
            );
        }

        return newValue;
    }

    public void RemoveObject(SyncedObject syncedObject)
    {
        for (LinkedListNode<HostedObject> node = m_HostedObjects.First; node != null; node = node.Next)
        {
            if (node.Value.Object == syncedObject)
            {
                m_HostedObjects.Remove(node);
                break;
            }
        }
    }

    struct HostedObject
    {
        public Int32 Id;
        public SyncedObject Object;

        public HostedObject(Int32 id, SyncedObject obj)
        {
            Id = id;
            Object = obj;
        }
    }

    LinkedList<HostedObject> m_HostedObjects;
}

class SyncedObject : IDisposable
{
    public SyncedObject(Int32 classId, SyncedObjectHost host)
    {
        m_ClassId = classId;
        m_Host = host;
        m_ObjectId = host.AddObject(this);
    }

    public void AddProperty(SyncedProperty syncedProperty)
    {
        m_Properties.Add(syncedProperty);
    }

    public Int32 ClassId
    {
        get { return m_ClassId; }
    }

    public Int32 ObjectId
    {
        get { return m_ObjectId; }
    }

    Int32 m_ClassId;
    SyncedObjectHost m_Host;
    Int32 m_ObjectId;
    List<SyncedProperty> m_Properties;

    void OnSpawn(PipeCom pipeCom)
    {
        pipeCom.Write((Int32)m_ClassId);
        pipeCom.Write((Int32)m_ObjectId);

        foreach (SyncedProperty synedProperty in m_Properties)
        {
            synedProperty.Sync(pipeCom);
        }
    }

    void OnSyncProperty(SyncedProperty syncedProperty, PipeCom pipeCom)
    {
        
    }
}

class SyncedProperty : SyncPoint
{
    public SyncedProperty(SyncedObject syncedObject, Int32 propertyId)
    {
        m_SyncedObject = syncedObject;

        syncedObject.AddProperty(this);
    }

    public virtual void Sync(PipeCom pipeCom)
    {
        pipeCom.Write(m_PropertyId);
    }

    protected void RequestSync()
    {

    }

    Int32 m_PropertyId;
    SyncedObject m_SyncedObject;
}

class SyncedInt32 : SyncedProperty
{
    public Int32 Value
    {
        get
        {
            return m_Value;
        }
        set
        {
            m_Value = value;
            RequestSync();
        }
    }

    public virtual void Sync(PipeCom pipeCom)
    {
        base.Sync(pipeCom);
        pipeCom.Write(m_Value);
    }

    Int32 m_Value;
}


class SyncPoint : IDisposable
{
    //
    // Public members:

    public void ConnectSyncer(Syncer syncer)
    {
        DisconnectSyncer();

        m_Syncer = syncer;
        if (m_UpdateRequested) Update();
    }

    public void DisconnectSyncer()
    {
        if (null != m_Syncer) m_Syncer.RemoveSyncPoint(this);
        m_Syncer = null;
    }

    public void Dispose()
    {
        DisconnectSyncer();
    }

    public virtual abstract void Sync(PipeCom pipeCom);

    //
    // Protected members:

    protected virtual abstract void OnSyncerConnect(PipeCom pipeCom);

    protected void RequestSync()
    {
        m_UpdateRequested = true;
        Update();
    }

    //
    // Private members:

    Syncer m_Syncer;
    bool m_UpdateRequested;

    void Update()
    {
        if (null != m_Syncer)
        {
            m_Syncer.AddSyncPoint(this);
            m_UpdateRequested = false;
        }
    }

}

class Syncer
{
    /// <summary>
    /// Adds a SyncPoint for a one-time call of Sync (it is removed from the list after the Sync).
    /// </summary>
    /// <param name="syncPoint">SyncPoint to queue for one Sync</param>
    public void AddSyncPoint(SyncPoint syncPoint)
    {
        Monitor.Enter(m_SyncPoints);
        try
        {
            if (!m_SyncPoints.Contains(syncPoint)) m_SyncPoints.AddLast(syncPoint);
        }
        finally
        {
            Monitor.Exit(m_SyncPoints);
        }
    }

    /// <summary>
    /// Allows to remove a SyncPoint early.
    /// </summary>
    /// <param name="syncPoint">SyncPoint to remove</param>
    public void RemoveSyncPoint(SyncPoint syncPoint)
    {
        Monitor.Enter(m_SyncPoints);
        try
        {
            m_SyncPoints.Remove(syncPoint);
        }
        finally
        {
            Monitor.Exit(m_SyncPoints);
        }
    }

    public void Sync(PipeCom pipeCom)
    {
        Monitor.Enter(m_SyncPoints);
        try
        {
            foreach (SyncPoint si in m_SyncPoints)
            {
                si.Sync(pipeCom);
            }
            m_SyncPoints.Clear();
        }
        finally
        {
            Monitor.Exit(m_SyncPoints);
        }
    }

    LinkedList<SyncPoint> m_SyncPoints;
}

} // namespace Afx {
*/