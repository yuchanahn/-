using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;
using TMPro;

public class SceneLoad : MonoBehaviour
{
    private SaveLoad sl;

    private void Awake()
    {
        sl = FindObjectOfType<SaveLoad>();
    }

    private void Start()
    {
        var d = sl.Load<save_ingame_data_t>();
        
        StartCoroutine(LoadScene(d is { IsNewGame: false } ? d.Value.scene_name : "YC_TUT_TEST"));
    }

    IEnumerator LoadScene(string sceneName)
    {
        AsyncOperation operation = SceneManager.LoadSceneAsync(sceneName);
        while (!operation.isDone)
        {
            yield return null;
        }
    }
}
