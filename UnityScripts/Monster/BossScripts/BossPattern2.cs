using System.Collections;
using System.Collections.Generic;
using System.Reflection;
using UnityEngine;
using UnityEngine.SocialPlatforms;

public class BossPattern2 : MonoBehaviour
{
    private Rigidbody2D rb;
    private RaycastHit2D rayHit;
    private BoxCollider2D col;
    private BoxCollider2D childCol;
    private Vector3 startPos;
    public Animator anim;
    private AudioSource audioSource;

    private float curAtkDelay;
    private bool isGrounded;
    private float randomDis;
    private int curAtkCount;
    private bool isAttack;
    private bool isCool;
    private float returnPosTime = 0;
    private float hp;
    private bool isEffect;

    [HideInInspector] public bool isPatternStart;
    public int patternOrder = 0;

    [Tooltip("최소 공격 거리")]
    [SerializeField] private float p2AtkDisMin;
    [Tooltip("최대 공격 거리")]
    [Header("최대 거리 거리 * 횟수 = 90 안넘어가게 해주세요.")]
    [SerializeField] private float p2AtkDisMax;
    [Space]
    [Tooltip("드랍 속도")]
    [SerializeField] private float p2DownSpd;
    [Header("30미만 설정")]
    [Tooltip("업 속도")]
    [SerializeField] private float p2UpSpd;
    [Tooltip("공격 딜레이")]
    [SerializeField] private float p2AtkDelay;
    [Tooltip("공격 데미지")]
    [SerializeField] private float p2Dmg;
    [Tooltip("공격 횟수")]
    [SerializeField] private int p2AtkCount;
    [Space]
    [Header("이펙트")]
    [SerializeField] private GameObject moveEffect;
    [SerializeField] private GameObject dropEffect;
    [Space]
    [Header("사운드")]
    [SerializeField] private AudioClip dropSound;

    private int animCount = 0;
    private Animator hammerAnim;

    private void Start()
    {
        rb = GetComponent<Rigidbody2D>();
        col = GetComponent<BoxCollider2D>();
        childCol = transform.GetChild(0).GetComponent<BoxCollider2D>();
        hammerAnim = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();

        startPos = transform.position;
        curAtkCount = p2AtkCount;
        rb.gravityScale = 4.0f;
        curAtkDelay = p2AtkDelay;
        isGrounded = true;
        isPatternStart = false;
        dropEffect.SetActive(false);
        moveEffect.SetActive(false);
    }

    private void Update()
    {
        hp = GetComponent<Health>().value;
        hammerAnim.SetBool("IsAttack", isAttack);

        // 1. 땅 체크
        GroundCheck();
        // 2. 몰?루 걍 해
        // ...
        // 3. 랜덤 거리 설정
        SetRandomDist();
        // 4. 이동
        Move();
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player") && isAttack)
        {
            collision.GetComponent<Health>().take_damage(p2Dmg, -transform.position);
        }
    }

    private void Init()
    {
        rb.gravityScale = 4.0f;
        isAttack = false;
        isGrounded = true;
        p2AtkCount = curAtkCount;
        isPatternStart = false;
        patternOrder = 0;
        p2AtkDelay = curAtkDelay;
        animCount = 0;
    }

    private void SetRandomDist()
    {
        randomDis = Random.Range(p2AtkDisMin, p2AtkDisMax);
    }

    private void GroundCheck()
    {
        // Ray 체크
        Debug.DrawRay(transform.position, Vector2.down * (col.size.y + 0.1f), Color.red);
        rayHit = Physics2D.Raycast(transform.position, Vector2.down, col.size.y + 0.1f, LayerMask.GetMask("Ground"));

        if (rayHit.collider != null)
        {
            isGrounded = true;
        }
        else
        {
            isGrounded = false;
        }
    }

    // 시간 체크 - WaitTime 코루틴으로 대체중
    private void AttackDelayCheck()
    {
        if (p2AtkDelay > 0)
        {
            p2AtkDelay -= Time.deltaTime;
        }
        else
        {
            p2AtkDelay = 0;
        }
    }

    private void Move()
    {

        switch (patternOrder)
        {
            case 0:
                // 땅에 있을 때 && 공격 딜레이가 끝났을 때
                if (p2AtkCount > 0)
                {

                    rb.gravityScale = 0f;
                    childCol.enabled = false;
                    // 위로 이동
                    isAttack = true;
                    if (hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("open") && hammerAnim.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1.0f || hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("damaged"))
                    {
                        if (animCount == 0)
                        {
                            anim.SetTrigger("DoAttackRight");
                            animCount++;
                        }
                        isEffect = false;
                        transform.Translate(new Vector2(0, p2UpSpd * Time.deltaTime));
                    }
                }

                break;

            case 1:
                // 우측이동
                animCount = 0;
                transform.Translate(new Vector2(20 * Time.deltaTime, 0));
                break;

            case 2:
                // 랜덤거리 왼쪽 이동
                dropEffect.SetActive(false);
                transform.position = new Vector2(transform.position.x - randomDis, transform.position.y);
                isPatternStart = true;
                patternOrder++;
                break;

            case 3:
                // transform 내리기
                //rb.gravityScale = 4.0f;
                childCol.enabled = true;
                if (!isGrounded)
                {
                    isAttack = true;
                    rb.gravityScale = p2DownSpd;
                    //transform.Translate(new Vector2(0, -p2DownSpd * Time.deltaTime * Time.deltaTime * forceSpeed));

                }
                else if (isGrounded)
                {
                    if (!isEffect)
                    {
                        dropEffect.SetActive(true);
                        GameManager.Instance.CameraShake();
                        PlaySound(dropSound);
                        isEffect = true;
                        p2AtkCount--;
                    }
                    
                    if (!isCool)
                    {
                        isCool = true;
                    }
                    if (p2AtkCount > 0)
                    {

                        p2AtkDelay = curAtkDelay;

                        StartCoroutine(WaitTime(p2AtkDelay));

                    }
                    else if (p2AtkCount <= 0)
                    {
                        isAttack = false;
                        if (hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("idle") || hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("damaged"))
                        {
                            returnPosTime += Time.deltaTime;
                            if (returnPosTime > 2)
                            {
                                moveEffect.SetActive(true);
                                transform.Translate(new Vector2(p2UpSpd * Time.deltaTime, 0));
                                if (transform.position.x >= startPos.x)
                                {
                                    Init();
                                    dropEffect.SetActive(false);
                                    moveEffect.SetActive(false);
                                    returnPosTime = 0f;
                                    isEffect = false;
                                    gameObject.GetComponent<BossPattern2>().enabled = false;
                                }
                            }
                        }
                    }
                }
                break;
        }
    }

    // 시간 기다리는 코루틴
    IEnumerator WaitTime(float time)
    {
        yield return new WaitForSeconds(time);
        isCool = false;
        patternOrder = 0;
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