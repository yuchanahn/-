using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;

public class BossPattern3 : MonoBehaviour
{
    private Rigidbody2D rb;
    private BoxCollider2D col;
    private BoxCollider2D childCol;
    private Vector3 startPos;
    public Animator anim;
    private AudioSource audioSource;

    private float curY;
    private bool isAttack;
    private bool isStun;
    private float upTimer;
    private float hp;

    [Tooltip("공격 속도")]
    [SerializeField] private float p3AtkSpd;
    [Tooltip("공격 데미지")]
    [SerializeField] private float p3Dmg;
    [Tooltip("")]
    [SerializeField] private float p3StopTime;
    [Tooltip("False : 좌, True : 우")]
    [SerializeField] private bool isRight;
    [Tooltip("다른 망치 오브젝트 할당")]
    [SerializeField] private GameObject otherHammer;
    [Space]
    [Header("이펙트")]
    [SerializeField] private GameObject punchHitEffect;
    [Space]
    [Header("사운드")]
    [SerializeField] private AudioClip punchSound;
    
    private Animator hammerAnim;
    private bool isEffect;
    [HideInInspector] public bool isPatternStart;
    public int patternOrder = 0;

    private void Start()
    {
        startPos = transform.position;
        rb = GetComponent<Rigidbody2D>();
        col = GetComponent<BoxCollider2D>();
        childCol = transform.GetChild(0).GetComponent<BoxCollider2D>();
        hammerAnim = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();
    }

    private void Update()
    {
        hp = GetComponent<Health>().value;
        hammerAnim.SetBool("IsAttack", isAttack);
        PatternState();
    }

    // 이동
    private void PatternState()
    {
        switch (patternOrder)
        {
            case 0: // 좌우 공격 이동


                if (isStun)
                {
                    patternOrder = 4;
                }
                if (isRight)
                {
                    isAttack = true;
                    if (hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("open") && hammerAnim.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1.0f || hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("damaged"))
                    {
                        isPatternStart = true;
                        if (otherHammer.GetComponent<BossPattern3>().isPatternStart)
                            transform.Translate(Vector3.left * p3AtkSpd * Time.deltaTime);
                    }
                }
                else
                {
                    isAttack = true;
                    if (hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("open") && hammerAnim.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1.0f || hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("damaged"))
                    {
                        isPatternStart = true;
                        if (otherHammer.GetComponent<BossPattern3>().isPatternStart)
                            transform.Translate(Vector3.right * p3AtkSpd * Time.deltaTime);
                    }
                }
                break;

            case 1: // 위로 이동
                upTimer += Time.deltaTime;
                if (upTimer > 2f)
                {
                    rb.gravityScale = 0.0f;
                    transform.position = Vector2.MoveTowards(transform.position, new Vector3(transform.position.x, transform.position.y + 50.0f), Time.deltaTime * p3AtkSpd);
                    if (transform.position.y >= curY + 50.0f)
                    {
                        upTimer = 0;
                        patternOrder++;
                    }
                }
                break;

            case 2: // 좌우 이동
                if (punchHitEffect != null)
                    punchHitEffect.SetActive(false);
                transform.position = Vector2.MoveTowards(transform.position, new Vector3(startPos.x, transform.position.y), Time.deltaTime * p3AtkSpd);
                if (transform.position.x == startPos.x) patternOrder++;
                break;

            case 3: // 아래로 이동

                transform.position = Vector2.MoveTowards(transform.position, startPos, Time.deltaTime * p3AtkSpd);
                if (transform.position.y <= startPos.y + .5f)
                {
                    isAttack = false;
                    rb.gravityScale = 4.0f;
                    //patternOrder = 0;
                    if (hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("idle") || hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("damaged"))
                    {
                        isPatternStart = false;
                        isEffect = false;
                        gameObject.GetComponent<BossPattern3>().enabled = false;
                    }
                }
                break;

            case 4: // 패링상태일시
                isAttack = false;
                break;
        }
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player") && isAttack)
        {
            collision.GetComponent<Health>().take_damage(p3Dmg, -transform.position);
            Debug.Log("패턴3 데미지");
        }
        if (collision.gameObject == otherHammer)
        {
            if (patternOrder == 0 || patternOrder == 4)
            {
                Debug.Log("OnTriggerEnter BossWeapon");
                if (punchHitEffect != null && !isEffect)
                {
                    punchHitEffect.SetActive(true);
                    GameManager.Instance.CameraShake();
                    PlaySound(punchSound);
                    isEffect = true;
                }
                //isAttack = true;
                isStun = false;
                curY = transform.position.y;
                patternOrder = 1;
            }
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
        }
    }
}
