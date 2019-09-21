using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Node
{
	public int m_id;
	public Vector2 m_position;
	public Vector2 m_dimension = new Vector2(150, 45);
	public Plug m_inputPlug;
	public Dictionary<int, Plug> m_outputPlugs; // key = plug.m_id
}

public enum PlugType { kIn, kOut }

public class Plug
{
	public PlugType m_plugType;
	public Vector2 m_position;
	public int m_plugId;
	public int m_nodeId;
}

public class Connection
{
	public int m_id;
	public int m_inputNodeId;
	public int m_outputNodeId;
	public int m_inputPlugId;
	public int m_outputPlugId;
	
	public Connection()
	{

	}

	public Connection(int inputNodeId, int inputPlugId, int outputNodeId, int outputPlugId)
	{
		m_inputNodeId = inputNodeId;
		m_inputPlugId = inputPlugId;
		m_outputNodeId = outputNodeId;
		m_outputPlugId = outputPlugId;
	}
}

public class NodeGraphModel
{
	private Dictionary<int, Node> m_nodes; // key = node.m_id
	private Dictionary<int, Connection> m_connections; // key = connection.m_id
	private Dictionary<int, DialogueData> m_dialogueData; // key = node_id	

	public float plug_height = 20.0f;
	public float in_between_plug_height = 10.0f;

	public NodeGraphModel()
	{
		m_nodes = new Dictionary<int, Node>();
		m_connections = new Dictionary<int, Connection>();
		m_dialogueData = new Dictionary<int, DialogueData>();
	}

	public int AddNode(Vector2 position) // returns created node id;
	{
		Node newNode = new Node();
		newNode.m_id = newNode.GetHashCode();
		newNode.m_position = position;
		newNode.m_inputPlug = new Plug();
		newNode.m_inputPlug.m_nodeId = newNode.m_id;
		newNode.m_inputPlug.m_plugId = newNode.m_inputPlug.GetHashCode();
		newNode.m_inputPlug.m_plugType = PlugType.kIn;
		newNode.m_outputPlugs = new Dictionary<int, Plug>();
		DialogueData dialogue = new DialogueData();
		dialogue.node_id = newNode.m_id;
		m_dialogueData.Add(newNode.m_id, dialogue);
		m_nodes.Add(newNode.m_id, newNode);

		return newNode.m_id;
	}

	public void AddNode(Node node)
	{
		m_nodes.Add(node.m_id, node);
	}

	public void RemoveNode(int node_id)
	{
		Node node_to_remove = GetNodeFromID(node_id);

		// removing connections related to the node
		List<Connection> connections_to_remove = new List<Connection>();
		foreach (KeyValuePair<int, Connection> connection_pair in m_connections)
		{
			Connection connection = connection_pair.Value;
			if (connection.m_inputNodeId == node_to_remove.m_id)
			{
				connections_to_remove.Add(connection);
			}
			else if (connection.m_outputNodeId == node_to_remove.m_id)
			{
				connections_to_remove.Add(connection);
			}
		}
		foreach (Connection connection in connections_to_remove)
		{
			RemoveConnection(connection.m_id);
		}

		// removing dialogue data assoiciated with the node
		m_dialogueData.Remove(node_id);
		// removing actual node
		m_nodes.Remove(node_id);
	}

	public int AddOutputPlugToNode(int node_id) // returns created plug id
	{
		Plug newOutPlug = new Plug();
		newOutPlug.m_nodeId = node_id;
		newOutPlug.m_plugId = newOutPlug.GetHashCode();
		newOutPlug.m_plugType = PlugType.kOut;
		GetNodeFromID(node_id).m_outputPlugs.Add(newOutPlug.m_plugId, newOutPlug);
		GetNodeFromID(node_id).m_dimension.y += in_between_plug_height + plug_height;
		return newOutPlug.m_plugId;
	}

	public void RemoveOutputPlugFromNode(int output_id, int node_id)
	{
		Node node = GetNodeFromID(node_id);
		Plug plug_to_remove;
		node.m_outputPlugs.TryGetValue(output_id, out plug_to_remove);

		// find connections related to plug being removed
		List<Connection> connections_to_remove = new List<Connection>();
		foreach (KeyValuePair<int, Connection> connection_pair in m_connections)
		{
			Connection connection = connection_pair.Value;
			if (connection.m_outputPlugId == output_id)
			{
				connections_to_remove.Add(connection);
			}
		}
		// remove found connections
		foreach (Connection connection in connections_to_remove)
		{
			RemoveConnection(connection.m_id);
		}

		// remova plug itself
		node.m_outputPlugs.Remove(output_id);
		// adjust node dimensions according to new plug count
		node.m_dimension.y -= in_between_plug_height + plug_height;
	}

	public Node GetNodeFromID(int node_id)
	{
		Node returnVal;
		m_nodes.TryGetValue(node_id, out returnVal);
		return returnVal;
	}

	public DialogueData GetDataFromNodeID(int node_id)
	{
		DialogueData data;
		m_dialogueData.TryGetValue(node_id, out data);
		return data;
	}

	public Dictionary<int, Node> GetNodes()
	{
		return m_nodes;
	}

	public Dictionary<int, Connection> GetConnections()
	{
		return m_connections;
	}

	public Dictionary<int, DialogueData> GetDialogueData()
	{
		return m_dialogueData;
	}

	public void AddDialogueData(DialogueData dialogue)
	{
		m_dialogueData.Add(dialogue.node_id, dialogue);
	}

	public int AddConnection(int inPlugId, int outPlugId, int inNodeId, int outNodeId) // returns created connection id
	{
		Connection newConnection = new Connection(inNodeId, inPlugId, outNodeId, outPlugId);
		newConnection.m_id = newConnection.GetHashCode();
		m_connections.Add(newConnection.m_id, newConnection);
		return newConnection.m_id;
	}

	public void AddConnection(Connection connection)
	{
		m_connections.Add(connection.m_id, connection);
	}

	public void RemoveConnection(int connectionId)
	{
		m_connections.Remove(connectionId);
	}
}
