using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ToScene : MonoBehaviour
{
    public void OnToScene(string sceneName) {
        UnityEngine.SceneManagement.SceneManager.LoadScene(sceneName);
    }
}
