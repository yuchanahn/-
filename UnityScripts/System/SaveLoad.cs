#nullable enable
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.VisualScripting;
using System.IO;
using Newtonsoft.Json.Bson;
using System;
using Newtonsoft.Json;



public struct save_ingame_data_t
{
    public int potion_count;
    public int hp;
    public int sp;
    public string scene_name;
    public bool IsNewGame;
    public bool scene_change;
    public bool reset;
}

public class SaveLoad : MonoBehaviour
{
    [SerializeField] private string filepath;

    public SaveLoad(string filepath)
    {
        this.filepath = filepath;
    }

    public T? Load<T>() where T : struct {
        try {
            using StreamReader file = new($"{filepath}/Save_{typeof(T).Name}.txt");
            return JsonConvert.DeserializeObject<T>(file.ReadToEnd());
        } catch {
            Debug.LogWarning("save-file ¾øÀ½.");
            return null;
        }
    }
    public void Save<T>(T data) where T : struct {
        using StreamWriter file = new($"{filepath}/Save_{typeof(T).Name}.txt");
        file.WriteLine(JsonConvert.SerializeObject(data, Formatting.Indented));
    }
}
