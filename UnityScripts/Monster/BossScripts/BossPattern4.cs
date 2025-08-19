using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossPattern4 : MonoBehaviour
{
    [Tooltip("틱 딜레이")]
    [SerializeField] private float p4TickDelay;
    [Tooltip("공격 데미지")]
    [SerializeField] private float p4TickDmg;
    [Tooltip("회전 속도")]
    [SerializeField] private float p4RotSpd;
    [Tooltip("시연시간")]
    [SerializeField] private float p4PlayTime;
    [Tooltip("True : 시계방향, False : 반시계방향")]
    [SerializeField] private bool p4RotDir;
    [Space]
    [Header("사운드")]
    [Tooltip("레이저 시작 사운드")]
    [SerializeField] private AudioClip p4LaserStartSound;
    [Tooltip("레이저 사운드")]
    [SerializeField] private AudioClip p4LaserSound;

    private SpriteRenderer _spriteRenderer;
    private AudioSource audioSource;
    private float curTimer;
    private bool isAttack;
    public Animator anim;

    public BoxCollider2D[] col;

    void Start()
    {
        _spriteRenderer = GetComponent<SpriteRenderer>();
        col = GetComponents<BoxCollider2D>();
        curTimer = p4PlayTime;
        audioSource = GetComponent<AudioSource>();

        _spriteRenderer.enabled = false;
        for (int i = 0; i < col.Length; i++)
        {
            col[i].enabled = false;
        }
    }

    void Update()
    {
        RotateLaser();
        anim.SetBool("DoLaser", isAttack);


    }

    // 트리거 엔터
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.CompareTag("Player") && isAttack)
        {
            collision.gameObject.GetComponent<Health>().take_damage(p4TickDmg);
        }
    }

    private void RotateLaser()
    {
        PlayTime();

        if (p4RotDir)
        {
            transform.Rotate(0, 0, p4RotSpd * Time.deltaTime);
        }
        else
        {
            transform.Rotate(0, 0, -p4RotSpd * Time.deltaTime);
        }
    }

    private void PlayTime()
    {
        isAttack = true;
        if (anim.GetCurrentAnimatorStateInfo(0).IsName("laser_start"))
        {
            if (audioSource.clip == p4LaserSound)
            {
                audioSource.Stop();
            }
            PlaySound(p4LaserStartSound);
        }
        else if (anim.GetCurrentAnimatorStateInfo(0).IsName("laser_ing"))
        {
            if (audioSource.clip == p4LaserStartSound)
            {
                audioSource.Stop();
            }
            for (int i = 0; i < col.Length; i++)
            {
                col[i].enabled = true;
            }
            _spriteRenderer.enabled = true;
            PlaySound(p4LaserSound);
        }

        p4PlayTime -= Time.deltaTime;
        if (p4PlayTime <= 0)
        {
            p4PlayTime = curTimer;
            isAttack = false;
            //gameObject.GetComponent<BossPattern4>().enabled = false;
            gameObject.transform.rotation = Quaternion.identity;
            for (int i = 0; i < col.Length; i++)
            {
                col[i].enabled = false;
            }
            _spriteRenderer.enabled = false;
            audioSource.Stop();
            audioSource.clip = null;
            gameObject.SetActive(false);
        }
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
