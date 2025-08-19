using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using UnityEngine;
using UnityEngine.Events;

public class UIScriptManager : MonoBehaviour
{
    public static UIScriptManager instance;
    public string file_name;
    public string script_name;
    public List<dialogue_data_t> dialogues = new List<dialogue_data_t>();
    
    public int idx = 0;

    [SerializeField] UnityEvent OnEnd;

    public UIDialogue dialogue;

    private void Awake() {
        instance = this;
        load(file_name, script_name);
    }
    
    public void Next() {
        if (idx == dialogues.Count) {
            OnEnd?.Invoke();
            return;
        }
        dialogue.cur_data = dialogues[idx++];
        dialogue.OnChenaged();
    }

    void load(string file_name, string script_name) {
        StreamReader sr = new StreamReader($"./Assets/{file_name}/{script_name}.txt");
        string text = sr.ReadToEnd();

        foreach(var m in text.Split("$")) {
            var d = m.Trim();
            if (d == "") continue;
            dialogue_data_t dialogue = new dialogue_data_t();
            dialogue.chracter_name = d.Split(':')[0];
            dialogue.text = d.Split(':')[1];
            dialogues.Add(dialogue);
        }
        
        sr.Close();
    }
}
