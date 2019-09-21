using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DialogueData
{
    public int node_id;
    public bool m_isStartNode;
    public string characterName = "";
    public string dialogueText = "";
    public string previewDialogueText = "";
    public bool m_isBranching;
    public List<int> m_nextDialogueData;
}
public class DialogueAsset : ScriptableObject
{
    public string m_assetData = ""; // for editor stuff
    public string m_dialogueRuntimeSaveData = ""; // for runtime stuff
    public Dictionary<int, DialogueData> m_dialogueData; // what the dialogue system will use
    public int m_startIndex;

    public DialogueAsset()
    {
        m_dialogueData = new Dictionary<int, DialogueData>();
    }

	// edit here when dialogue data changes
	public void LoadRuntimeSaveData()
    {
        m_dialogueData = new Dictionary<int, DialogueData>();

        string[] data = m_dialogueRuntimeSaveData.Split("`".ToCharArray());
        int data_index = 0;
        if (data.Length <= 1)
            return;

        // load dialogue data (runtime)
        int dialogue_count = int.Parse(data[data_index++]);
        for (int dialogue_index = 0; dialogue_index < dialogue_count; ++dialogue_index)
        {
            DialogueData dialogue = new DialogueData();
            dialogue.node_id = int.Parse(data[data_index++]);
            dialogue.characterName = data[data_index++];
            dialogue.dialogueText = data[data_index++];
            dialogue.previewDialogueText = data[data_index++];
            dialogue.m_isBranching = bool.Parse(data[data_index++]);

            dialogue.m_nextDialogueData = new List<int>();
            int branchingIDs_count = int.Parse(data[data_index++]);
            for (int i = 0; i < branchingIDs_count; ++i)
            {
                dialogue.m_nextDialogueData.Add(int.Parse(data[data_index++]));
            }

			m_dialogueData.Add(dialogue.node_id, dialogue);
        }
        m_startIndex = int.Parse(data[data_index++]);
	}
}
