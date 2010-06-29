﻿// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-19 by dominik.matrixstorm.com
//
// First changes:
// 2008-12-23 by dominik.matrixstorm.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Xml;
using System.Threading;

namespace AfxGui
{
    interface IUpdateCheckResult
    {
        bool IsUpdated { get; }
        Uri Uri { get; }
    }

    /// <param name="checkResult">null in case the check failed, otherise the result of the check</param>
    delegate void UpdateCheckedDelegate(object o, IUpdateCheckResult checkResult);

    class UpdateCheckNotificationTarget
    {
        public UpdateCheckNotificationTarget(System.Windows.Forms.Control target, UpdateCheckedDelegate updatedChecked)
        {
            m_UpdateChecked = updatedChecked;
            m_Target = target;
        }

        public void Notify(object o, IUpdateCheckResult checkResult)
        {
            m_Target.Invoke(m_UpdateChecked, new object[]{o, checkResult});
        }

        UpdateCheckedDelegate m_UpdateChecked;
        System.Windows.Forms.Control m_Target;

    }

    // UpdateCheck /////////////////////////////////////////////////////////////

    class UpdateCheck :
        IDisposable
    {
        //
        // Public members:

        public UpdateCheck()
        {
	        m_CheckThreadQuit = false;
	        m_CheckThreadWork = new AutoResetEvent(false);
            m_Disposed = false;

            m_Guids = new Guid[]{
                // always put the current guid first.
                new Guid("a0e4a9cc-32c0-40e7-890f-e1c22855f52d"),
                new Guid("4a1bef0d-1d3f-4b4a-adfa-c1159a23bab6")
            };

	        m_Targets = new LinkedList<UpdateCheckNotificationTarget>();

	        m_CheckThread = new Thread(new ThreadStart(CheckWorker));
	        m_CheckThread.Name = "hlae Updater CheckThread";
        }

	    /// <summary> Triggers a new (asynchronus) update check. </summary>
        public void StartCheck()
        {
            if (!m_CheckThread.IsAlive)
                m_CheckThread.Start();
            else
                m_CheckThreadWork.Set();
        }

	    /// <summary> Eventhandler (re-)triggered when a updatecheck completed </summary>
        public void BeginCheckedNotification(UpdateCheckNotificationTarget target)
        {
	        try {
                Monitor.Enter(m_Targets);

                m_Targets.AddLast(target);
	        }
	        finally {
                Monitor.Exit(m_Targets);
	        }
        }

        public void Dispose()
        {
            if (m_Disposed)
                return;

            m_Disposed = true;

            while (m_CheckThread.IsAlive)
            {
                m_CheckThreadQuit = true;
                m_CheckThreadWork.Set();
                m_CheckThread.Join(250);
            }
        }

	    /// <remarks> Do not call upon notification (will cause deadlock). </remarks>
        public void EndCheckedNotification(UpdateCheckNotificationTarget target)
        {
	        try {
                Monitor.Enter(m_Targets);

                m_Targets.Remove(target);
	        }
	        finally {
                Monitor.Exit(m_Targets);
	        }
        }

	    //
	    // Public properties:

        /// <summary>
        /// Current Guid (that identifies the current version).
        /// </summary>
	    public Guid Guid
        {
            get
            {
                return m_Guids[0];
            }
	    }

        //
        // Private members:

        class UpdateInfo :
            IUpdateCheckResult
        {
            public UpdateInfo(bool isUpdated, Uri uri)
            {
                m_IsUpdated = isUpdated;
                m_Uri = uri;
            }

            public static UpdateInfo Get(Guid[] guids, String url, int maxRedirects)
            {
                Uri uri = null;
                Guid guid = Guid.Empty;
                
                while (true)
	            {
	                XmlDocument doc;
	                HttpWebRequest request;
	                HttpWebResponse response = null;
                    Stream stream = null;

                    try {
			            request = WebRequest.Create( url ) as HttpWebRequest;
			            request.MaximumAutomaticRedirections = 1;
			            request.AllowAutoRedirect = true;
			            request.Timeout = 10000;
			            response = request.GetResponse() as HttpWebResponse;

			            doc = new XmlDocument();
			            stream = response.GetResponseStream();
			            doc.Load( stream );

			            XmlNode anode;
			            XmlNode nodeUpdate = doc.SelectSingleNode("update");
			            XmlAttribute attr = nodeUpdate.Attributes["redirect"];

			            if(null == attr)
                        {
                            // no more redirects.

				            guid = new Guid(nodeUpdate["guid"].InnerText);
				            uri = null != (anode = nodeUpdate["link"]) ? new Uri(anode.InnerText) : null;
				            break;
			            }
			            else if(0 < maxRedirects)
                        {
				            // follow redirect.

				            maxRedirects--;
				            url = attr.InnerText;
			            }
			            else
				            throw new System.ApplicationException();
		            }
		            catch(Exception)
                    {
                        uri = null;
		            }

                    if (null != stream) stream.Close();
                    if (null != response) response.Close();
	            }

                if (null == uri)
                    return null;

                return new UpdateInfo(
                    0 > Array.IndexOf<Guid>(guids, guid), // Is updated if the guid is not known to us, remember that Guid is a value class.
                    uri
                );
            }

            //
            // Interface implementations

	        bool IUpdateCheckResult.IsUpdated
            {
                get
                {
                    return m_IsUpdated;
                }
            }

            Uri IUpdateCheckResult.Uri
            {
                get
                {
                    return m_Uri;
                }
	        }

            //
            //  Private members:
	        bool m_IsUpdated;
	        Uri m_Uri;
        }

        const int m_MaxRedirects = 1;
        const String m_Url = "http://update.matrixstorm.com/61b65ac26b714c41a1d998af3c5bd6dd.xml";

	    Thread m_CheckThread;
	    bool m_CheckThreadQuit;
	    AutoResetEvent m_CheckThreadWork;
        bool m_Disposed;
        Guid[] m_Guids;
        LinkedList<UpdateCheckNotificationTarget> m_Targets;

        void CheckWorker()
        {
	        while(!m_CheckThreadQuit)
	        {
                IUpdateCheckResult checkResult = UpdateInfo.Get(m_Guids, m_Url, m_MaxRedirects);

		        try {
			        Monitor.Enter(m_Targets);

			        for(
				        LinkedListNode<UpdateCheckNotificationTarget> cur = m_Targets.First;
				        null != cur;
				        cur = cur.Next
			        )
                        cur.Value.Notify(this, checkResult);
		        }
		        finally {
			        Monitor.Exit(m_Targets);
		        }

		        m_CheckThreadWork.WaitOne();
	        }
        }

    }
}