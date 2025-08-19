using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class BossPattern5 : MonoBehaviour
{
    [Header("망치 할당")]
    [SerializeField] private GameObject hammer1;
    [SerializeField] private GameObject hammer2;

    [SerializeField] private float p5Dmg;
    [SerializeField] private int p5Count;
    [SerializeField] private float p5ChargeTime;
    [SerializeField] private float p5AtkTime;

    [Space]
    [Header("사운드")]
    [SerializeField] private AudioClip p5ChargeSound;

    private bool isAttack;
    private BoxCollider2D col;
    private float timer;
    private float atkTimer;
    private int saveAtkCount;
    private SpriteRenderer killZone;
    public Animator anim;
    public Animator[] hammerAnim;
    private AudioSource audioSource;

    void Start()
    {
        col = GetComponent<BoxCollider2D>();
        killZone = GetComponent<SpriteRenderer>();
        killZone.enabled = false;
        col.enabled = false;
        isAttack = true;
        saveAtkCount = p5Count;
        audioSource = GetComponent<AudioSource>();
    }

    private void Update()
    {
        for (int i = 0; i < hammerAnim.Length; i++)
        {
            hammerAnim[i].SetBool("IsAttack", isAttack);
            
            if (hammerAnim[i].GetCurrentAnimatorStateInfo(0).IsName("open") && hammerAnim[i].GetCurrentAnimatorStateInfo(0).normalizedTime >= 1.0f)
            {
                StartCoroutine(delayAttack(p5ChargeTime, p5AtkTime, p5Count));
            }
        }
    }

    private void OnTriggerStay2D(Collider2D collision)
    {
        if (collision.CompareTag("Player"))
        {
            collision.GetComponent<Health>().take_damage(p5Dmg);
        }
    }


    private void P5Logic()
    {
        timer += Time.deltaTime;

        if (p5Count <= 0)
        {
            saveAtkCount = p5Count;
            gameObject.SetActive(false);
        }

        if (timer > p5ChargeTime && p5Count > 0)
        {
            col.enabled = true;
            killZone.enabled = true;

            timer = 0;
        }
    }

    IEnumerator delayAttack(float charge, float delay, float count)
    {
        anim.SetTrigger("DoLaser");
        // 카운트가 0 이상일 때
        if (count > 0)
        {
            // 차징시간 대기
            yield return new WaitForSeconds(charge);
            // 공격 시작
            PlaySound(p5ChargeSound);
            col.enabled = true;
            killZone.enabled = true;
            // 카운트 감소
            count--;
            // 공격시간 대기
            yield return new WaitForSeconds(delay);
            // 공격 종료
            col.enabled = false;
            killZone.enabled = false;
            // 다시 시작
            StartCoroutine(delayAttack(charge, delay, count));
        }
        // 카운트가 0 이하일 때
        else if (count <= 0)
        {
            // 카운트 초기화
            count = saveAtkCount;
            // 로직 끝
            yield return null;
        }
    }

    private void PlaySound(AudioClip clip, bool bLoop = false)
    {
        if (clip != null && !audioSource.isPlaying)
        {
            audioSource.clip = clip;
            audioSource.loop = bLoop;
            audioSource.Play();
            Debug.Log(audioSource.clip);
        }
    }
}
