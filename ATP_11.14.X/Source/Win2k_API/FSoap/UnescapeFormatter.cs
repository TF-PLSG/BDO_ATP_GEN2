using System;
using System.IO;
using System.Reflection;
using System.Globalization;
using System.Collections;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Proxies;
using System.Runtime.Remoting.Messaging;
using System.Runtime.Serialization;
namespace FSOAP
{
	/// <summary>
	/// Summary description for UnescapeFormatter.
	/// </summary>
	public class UnescapeFormatter : IRemotingFormatter
	{
		internal ISurrogateSelector m_surrogates;
		internal SerializationBinder m_binder;
		internal StreamingContext m_context;

		public UnescapeFormatter()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		public Object Deserialize(Stream serializationStream)
		{
			return Deserialize(serializationStream, null);
		}

		public object Deserialize( Stream stream, HeaderHandler headerHandler )
		{
			return stream;
		}

		public void Serialize(Stream serializationStream,Object graph)
		{
			Serialize(serializationStream, graph, null);
		}

		public void Serialize( Stream stream, object obj, Header[] headers )
		{
			Console.WriteLine("\n\nSerializing\n\n");
		}

		public SerializationBinder Binder 
		{
			get { return m_binder; }
			set { m_binder = value; } 
		}
		public StreamingContext Context 
		{
			get { return m_context;  } 
			set { m_context = value; } 
		}

		public ISurrogateSelector SurrogateSelector 
		{
			get { return m_surrogates;  }
			set { m_surrogates = value; }
		}
	}
}
