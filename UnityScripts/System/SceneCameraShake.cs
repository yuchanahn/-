using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneCameraShake : MonoBehaviour
{
    [SerializeField] private float shakeNextTime = 2f;

    [Range(0.01f, 0.1f)] public float shakeRange = 0.05f;
    [Range(0.1f, 1f)] public float duration = 0.5f;

    // Update is called once per frame
    void Start()
    {
        StartCoroutine(WhileStartShake(shakeNextTime));
    }
    
    IEnumerator WhileStartShake(float delay)
    {
        yield return new WaitForSeconds(delay);
        GameManager.Instance.shakeRange = this.shakeRange;
        GameManager.Instance.duration = this.duration;
        GameManager.Instance.CameraShake();
        GameManager.Instance.shakeRange = GameManager.Instance.originshakeRange;
        GameManager.Instance.duration = GameManager.Instance.originDuration;
        Debug.Log("Shake!");
        StartCoroutine(WhileStartShake(delay));
    }
}
