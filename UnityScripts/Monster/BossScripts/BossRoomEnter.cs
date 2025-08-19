using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Cinemachine;

public class BossRoomEnter : MonoBehaviour
{
    CinemachineVirtualCamera vcam;
    [SerializeField] private GameObject boss;
    [SerializeField] private float startBossStateTime;
    [Space]
    [Header("사운드")]
    [SerializeField] private AudioClip bossRoomBGM;
    private bool startState;
    public GameObject bossRoomDoor;
    private AudioSource audioSource;

    private void Start()
    {
        bossRoomDoor = transform.GetChild(0).gameObject;
        audioSource = GetComponent<AudioSource>();
    }

    private void Update()
    {
        if (startState)
        {
            gameObject.GetComponent<BoxCollider2D>().isTrigger = false;
            bossRoomDoor.transform.position = Vector3.MoveTowards(bossRoomDoor.transform.position, new Vector3(bossRoomDoor.transform.position.x, 4.05f, bossRoomDoor.transform.position.z), 50f * Time.deltaTime);
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            vcam = GameObject.Find("CM vcam1").GetComponent<CinemachineVirtualCamera>();
            vcam.Follow = boss.transform;
            var transposer = vcam.GetCinemachineComponent<CinemachineTransposer>();
            transposer.m_FollowOffset = new Vector3(0, 5.3f, -11.97f);
            StartCoroutine(StartBossPattern(startBossStateTime));
            //gameObject.SetActive(false);
        }
    }

    IEnumerator StartBossPattern(float delay)
    {
        yield return new WaitForSeconds(delay);
        PlaySound(bossRoomBGM, true);
        startState = true;
        boss.GetComponent<BossState>().enabled = true;
    }

    private void PlaySound(AudioClip clip, bool bLoop = false)
    {
        if (bLoop)
            audioSource.loop = true;
        else if (!bLoop)
            audioSource.loop = false;

        if (clip != null && !audioSource.isPlaying)
        {
            audioSource.clip = clip;
            audioSource.Play();
            Debug.Log("사운드 재생 : " + clip.name);
        }
    }
}