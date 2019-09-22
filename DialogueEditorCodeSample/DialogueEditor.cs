using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class DialogueEditor : EditorWindow
{
    private DialogueAssetBuilder m_dialogueAssetBuilder;

    private Rect m_nodePropertyPanel;
    private Rect m_nodeGraphPanel;
    private Rect m_resizer;

    private NodeGraphView m_nodeGraphView;
    private NodePropertiesView m_nodePropertiesView;

    private float m_graphPanelSizeRatio = 0.8f;
    private bool m_isResizing;
    private bool m_isNodeSelected;

    private GUIStyle resizerStyle;

    public DialogueEditor()
    {
    }

    [MenuItem("Window/Dialogue Editor")]
    private static void OpenWindow()
    {
        DialogueEditor window = CreateWindow<DialogueEditor>();
        window.titleContent = new GUIContent("Dialogue Editor");
    }

    [MenuItem("Assets/Create/Dialogue Asset")]
    public static void CreateAsset()
    {
        DialogueAsset dialogue_asset = CreateInstance<DialogueAsset>();
        string asset_path = AssetDatabase.GetAssetPath(Selection.activeObject);
        if (asset_path == "")
        {
            asset_path = "Assets/Dialogue";
        }

        asset_path = AssetDatabase.GenerateUniqueAssetPath(asset_path + "/New" + typeof(DialogueAsset).ToString() + ".asset");
        Debug.Log(asset_path);
        AssetDatabase.CreateAsset(dialogue_asset, asset_path);

        AssetDatabase.SaveAssets();
        Selection.activeObject = dialogue_asset;
    }


    [UnityEditor.Callbacks.OnOpenAsset(1)]
    public static bool OnOpenAsset(int instanceID, int line)
    {
        if (Selection.activeObject as DialogueAsset != null)
        {
            DialogueEditor window = CreateWindow<DialogueEditor>();
            DialogueAsset asset = Selection.activeObject as DialogueAsset;
            window.titleContent = new GUIContent("Dialogue Editor");
            window.m_dialogueAssetBuilder = CreateInstance<DialogueAssetBuilder>();
            window.m_dialogueAssetBuilder.m_dialogueAsset = asset;
            window.m_dialogueAssetBuilder.LoadEditorSaveData();
            window.m_dialogueAssetBuilder.m_dialogueAsset = asset;
            return true;
        }
        return false;
    }

    private void OnEnable()
    {
        resizerStyle = new GUIStyle();
        resizerStyle.normal.background = EditorGUIUtility.Load("icons/d_AvatarBlendBackground.png") as Texture2D;
        m_nodeGraphView = new NodeGraphView();
        m_nodePropertiesView = new NodePropertiesView();
    }

    private void OnGUI()
    {
        DrawNodeGraphPanel();
        DrawNodePropertyPanel();
        DrawResizer();

        ProcessNodeEvents(Event.current);
        ProcessEvents(Event.current);
        if (GUI.changed) Repaint();
    }

    private void ProcessNodeEvents(Event e)
    {
        if (m_nodeGraphView.ProcessEvents(e, m_dialogueAssetBuilder))
        {
            GUI.changed = true;
        }
    }

    private void DrawNodePropertyPanel()
    {
        m_nodePropertyPanel = new Rect(position.width * m_graphPanelSizeRatio, 0, position.width * (1 - m_graphPanelSizeRatio), position.height);
        m_nodePropertiesView.DrawNodeProperties(m_nodePropertyPanel, m_nodeGraphView.GetSelectedNode(), m_dialogueAssetBuilder);
    }

    private void DrawNodeGraphPanel()
    {
        m_nodeGraphPanel = new Rect(0, 0, position.width * m_graphPanelSizeRatio, position.height);
        m_nodeGraphView.DrawNodeGraph(m_nodeGraphPanel, m_dialogueAssetBuilder);
    }

    private void DrawResizer()
    {
        m_resizer = new Rect(position.width * m_graphPanelSizeRatio - 5f, 0, 10f, position.height);
        GUILayout.BeginArea(new Rect(m_resizer.position + (Vector2.right * 5f), new Vector2(2, position.height)), resizerStyle);
        GUILayout.EndArea();

        EditorGUIUtility.AddCursorRect(m_resizer, MouseCursor.ResizeHorizontal);
    }

    private void ProcessEvents(Event e)
    {
        switch (e.type)
        {
            case EventType.MouseDown:
                if (e.button == 0 && m_resizer.Contains(e.mousePosition))
                {
                    m_isResizing = true;
                }
                break;
            case EventType.MouseUp:
                m_isResizing = false;
                break;
        }

        Resize(e);
    }

    private void Resize(Event e)
    {
        if (m_isResizing)
        {
            m_graphPanelSizeRatio = e.mousePosition.x / position.width;
            Repaint();
        }
    }
}
