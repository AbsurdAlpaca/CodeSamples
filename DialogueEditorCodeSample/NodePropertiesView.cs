using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class NodePropertiesView : GUILayout
{
	Rect m_nodePropertiesRect;
	NodeGraphModel m_nodeGraphModel = null;
	public NodePropertiesView()
	{
	}

	public void DrawNodeProperties(Rect propertiesRect, Node selected_node, DialogueAssetBuilder asset)
	{
		if (asset != null)
			m_nodeGraphModel = asset.m_nodeGraphModel;

		m_nodePropertiesRect = new Rect(propertiesRect.position.x + 5f, propertiesRect.position.y + 5f, propertiesRect.width - 10f, propertiesRect.height - 10f);
		BeginArea(m_nodePropertiesRect);
		if (selected_node != null) // if there has been a node selected, display that node's properties
		{
			int node_id = selected_node.m_id;
			Label("Node Properties");
			DialogueData data = m_nodeGraphModel.GetDataFromNodeID(node_id);
			if (data != null)
			{
                data.m_isStartNode = Toggle(data.m_isStartNode, "Is start node");
				Label(" ");

				Label("Character Speaking");
				data.characterName = TextArea(data.characterName);

                Label("Preview Text");
                data.previewDialogueText = TextArea(data.previewDialogueText, Height(50));

				Label("Dialogue Text");
				data.dialogueText = TextArea(data.dialogueText, Height(m_nodePropertiesRect.height * 0.5f));
			}

            if (m_nodeGraphModel != null)
            {
                if (Button("Add Dialogue Option"))
                {
                    m_nodeGraphModel.AddOutputPlugToNode(node_id);
                }
            }
		}
		else if (asset != null) // display asset properties
		{
			Label("Dialogue Asset");
			if (Button("Save Asset"))
			{
				// save asset
				asset.SaveAsset(m_nodeGraphModel);
			}
		}
		EndArea();
	}
}
