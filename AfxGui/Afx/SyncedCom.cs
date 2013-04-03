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

using System;
using System.Collections.Generic;
using System.Threading;

namespace Afx {

enum SyncedObjectAction
{
    NoMoreObjects = 0,
    Delete,
    Create,
    Property
}

class SyncedObjectHost
{
    public void AddObject(SyncedObject syncedObject)
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

        syncedObject.ObjectId = newValue;
        
        if(null != m_Syncer) syncedObject.ConnectSyncer(m_Syncer);
    }

    public virtual void ConnectSyncer(Syncer syncer)
    {
        m_Syncer = syncer;

        foreach (HostedObject hostedObject in m_HostedObjects)
        {
            hostedObject.Object.ConnectSyncer(syncer);
        }
    }

    public virtual void DisconnectSyncer()
    {
        foreach (HostedObject hostedObject in m_HostedObjects)
        {
            hostedObject.Object.DisconnectSyncer();
        }

        m_Syncer = null;
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

    LinkedList<HostedObject> m_HostedObjects = new LinkedList<HostedObject>();
    Syncer m_Syncer;
}

class SyncedObject : SyncPoint, IDisposable
{
    public SyncedObject(SyncedObjectHost host, Int32 classId)
    {
        m_ClassId = classId;
        m_Host = host;
        
        host.AddObject(this);
    }

    public void AddProperty(SyncedProperty syncedProperty)
    {
        m_Properties.Add(syncedProperty);

        if (null != m_Syncer) syncedProperty.ConnectSyncer(m_Syncer);
    }

    public override void ConnectSyncer(Syncer syncer)
    {
        base.ConnectSyncer(syncer);

        foreach (SyncedProperty syncedProperty in m_Properties)
        {
            syncedProperty.ConnectSyncer(syncer);
        }
    }

    public override void DisconnectSyncer()
    {
        foreach (SyncedProperty syncedProperty in m_Properties)
        {
            syncedProperty.DisconnectSyncer();
        }

        base.DisconnectSyncer();
    }

    public void Dispose()
    {
        if (m_Disposed) return;

        m_Disposed = true;

        m_Host.RemoveObject(this);
        RequestSync();
    }

    public override void Sync(PipeCom pipeCom)
    {
        if (m_Disposed)
        {
            pipeCom.Write((Int32)SyncedObjectAction.Delete);
            pipeCom.Write((Int32)m_ObjectId);
        }
        else
        {
            pipeCom.Write((Int32)SyncedObjectAction.Create);
            pipeCom.Write((Int32)m_ClassId);
            pipeCom.Write((Int32)m_ObjectId);
        }
    }

    public Int32 ClassId
    {
        get { return m_ClassId; }
    }

    public Int32 ObjectId
    {
        get { return m_ObjectId; }
        set { m_ObjectId = value; }
    }

    Int32 m_ClassId;
    bool m_Disposed;
    SyncedObjectHost m_Host;
    Int32 m_ObjectId;
    List<SyncedProperty> m_Properties = new List<SyncedProperty>();

}

class SyncedProperty : SyncPoint
{
    public SyncedProperty(SyncedObject syncedObject, Int32 propertyId)
    {
        m_SyncedObject = syncedObject;
        m_PropertyId = propertyId;

        syncedObject.AddProperty(this);
    }

    public override void Sync(PipeCom pipeCom)
    {
        pipeCom.Write((Int32)SyncedObjectAction.Property);
        pipeCom.Write((Int32)m_SyncedObject.ClassId);
        pipeCom.Write((Int32)m_SyncedObject.ObjectId);
        pipeCom.Write(m_PropertyId);
    }

    Int32 m_PropertyId;
    SyncedObject m_SyncedObject;
}

class SyncedBoolean : SyncedProperty
{
    public SyncedBoolean(SyncedObject syncedObject, Int32 propertyId)
        : base(syncedObject, propertyId)
    {
    }

    public Boolean Value
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

    public override void Sync(PipeCom pipeCom)
    {
        base.Sync(pipeCom);
        pipeCom.Write((Boolean)m_Value);
    }

    Boolean m_Value;
}

class SyncedInt32 : SyncedProperty
{
    public SyncedInt32(SyncedObject syncedObject, Int32 propertyId)
    : base(syncedObject, propertyId)
    {
    }

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

    public override void Sync(PipeCom pipeCom)
    {
        base.Sync(pipeCom);
        pipeCom.Write((Int32)m_Value);
    }

    Int32 m_Value;
}


abstract class SyncPoint
{
    //
    // Public members:

    public virtual void ConnectSyncer(Syncer syncer)
    {
        DisconnectSyncer();

        m_Syncer = syncer;
        RequestSync();
    }

    public virtual void DisconnectSyncer()
    {
        if (null != m_Syncer) m_Syncer.RemoveSyncPoint(this);
        m_Syncer = null;
    }

    public abstract void Sync(PipeCom pipeCom);

    //
    // Protected members:

    protected Syncer m_Syncer;

    protected void RequestSync()
    {
        if (null != m_Syncer)
        {
            m_Syncer.AddSyncPoint(this);
        }
    }

    //
    // Private members:
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

        pipeCom.Write((Int32)SyncedObjectAction.NoMoreObjects);
    }

    LinkedList<SyncPoint> m_SyncPoints = new LinkedList<SyncPoint>();
}

} // namespace Afx {
