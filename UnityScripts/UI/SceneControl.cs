using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneControl : MonoBehaviour
{
    private SavePlayerDataForScene sc;
    private void Awake()
    {
        sc = FindObjectOfType<SavePlayerDataForScene>();
    }

    public void LoadScene(string sceneName) {

        if (GameManager.Instance.enemyCount <= 0)
        {
            if(sc is not null && sceneName.Contains("Scene")) sc.save_scene(sceneName);
            UnityEngine.SceneManagement.SceneManager.LoadScene(sceneName);
        }
    }

    public void LoadSceneWithoutGameManager(string SceneName)
    {
        UnityEngine.SceneManagement.SceneManager.LoadScene(SceneName);
    }
}
