using System;
using System.Collections;
using System.Collections.Generic;
using System.Google;
using CutScene;
using UnityEngine;

public class CutsceneView : MonoBehaviour
{
    [SerializeField] private LoadGoogleSheets loadGoogleSheets;
    [SerializeField] private PrintStory cutscene;
    [SerializeField] string sheetName;
    private int index;
    [SerializeField] private int start_index;
    [SerializeField] private int end_index;

    public bool IsPlaying => !cutscene.isEnded;
    private void Awake()
    {
        index = start_index - 1;
        loadGoogleSheets.LoadDB(sheetName, x=>DB.GameDB.Story = x);
        loadGoogleSheets.LoadDB("NPC", x=>DB.GameDB.AllData["NPC"] = x);
        loadGoogleSheets.LoadDB("Intro_Cutscene", x=>DB.GameDB.AllData["Intro_Cutscene"] = x);
    }

    public void OnNext()
    {
        if (index >= end_index) return;
        cutscene.Print(++index);
    }
    
    public void OnPrev()
    {
        if (index <= start_index) return;
        cutscene.Print(--index);
    }
}
