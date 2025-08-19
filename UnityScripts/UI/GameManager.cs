using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using Cinemachine;

public class GameManager : MonoBehaviour
{
    private static GameManager instance = null;

    [HideInInspector] public int enemyCount;
    private EnemyBehavior[] enemyList;

    public Button mainMenuBtn;

    private CinemachineVirtualCamera vcam;
    CinemachineTransposer transposer;
    private Vector3 vcamOffset;

    /// <summary>
    /// 0.01 ~ 0.1의 값을 할당
    /// </summary>
    [Range(0.01f, 0.1f)] public float shakeRange = 0.05f;
    /// <summary>
    /// 0.1 ~ 1의 값을 할당
    /// </summary>
    [Range(0.1f, 1f)] public float duration = 0.5f;

    public float timeScaleItensity = 0.1f;
    public float timeScaleDuration = 0.5f;

    [HideInInspector] public float originshakeRange;
    [HideInInspector] public float originDuration;

    private void Awake()
    {
        if (instance != null)
        {
            Destroy(gameObject);
            return;
        }
        if (instance == null)
        {
            instance = this;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        if (mainMenuBtn != null)
        {
            mainMenuBtn.onClick.AddListener(() => { UnityEngine.SceneManagement.SceneManager.LoadScene("MainMenu"); });
        }

        vcam = GameObject.Find("CM vcam1").GetComponent<CinemachineVirtualCamera>();
        transposer = vcam.GetCinemachineComponent<CinemachineTransposer>();

        enemyList = GameObject.FindObjectsOfType<EnemyBehavior>();

        enemyCount = enemyList.Length;
        Debug.Log("적 개수 : " + enemyCount);

        if(timeScaleDuration > duration)
            timeScaleDuration = duration;

        originshakeRange = shakeRange;
        originDuration = duration;
    }

    public static GameManager Instance
    {
        get
        {
            if (instance == null)
            {
                return null;
            }
            return instance;
        }
    }

    /// <summary>
    /// 수치 변경시 GameManager.Instance의 shakeRange와 duration 변경 필요
    /// </summary>
    public void CameraShake()
    {
        vcamOffset = transposer.m_FollowOffset;
        InvokeRepeating("StartShake", 0f, 0.005f);
        Invoke("StopShake", duration);
    }

    public void CameraShaeWithoutStop()
    {
        vcamOffset = transposer.m_FollowOffset;
        InvokeRepeating("StartShake", 0f, 0.005f);
    }

    public void StartShake()
    {
        float cameraPosX = Random.value * shakeRange * 2 - shakeRange;
        float cameraPosY = Random.value * shakeRange * 2 - shakeRange;
        Vector3 setPos = transposer.m_FollowOffset;
        setPos.x += cameraPosX;
        setPos.y += cameraPosY;
        transposer.m_FollowOffset = setPos;
    }

    public void StopShake()
    {
        CancelInvoke("StartShake");
        transposer.m_FollowOffset = vcamOffset;
    }

    // 타임스케일 조정
    public void StartTimeScale(float timeScale)
    {
        Time.timeScale = timeScale;
    }

    public IEnumerator StopTimeScale()
    {
        yield return new WaitForSecondsRealtime(timeScaleDuration);
        Time.timeScale = 1f;
    }

    public IEnumerator StopTimeScale(float delay)
    {
        yield return new WaitForSecondsRealtime(delay);
        Time.timeScale = 1f;
    }
}