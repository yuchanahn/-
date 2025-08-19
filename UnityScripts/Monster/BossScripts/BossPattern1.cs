using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossPattern1 : MonoBehaviour
{
    private Vector3 playerPos;
    private Rigidbody2D rb;
    private RaycastHit2D rayHit;
    private BoxCollider2D col;
    private BoxCollider2D childCol;
    public Animator anim;
    private Hammer ham;
    private AudioSource audioSource;

    private bool isParrying;
    private float curCooltime;
    private bool isGrounded;
    private bool isReturn;
    private int curAttackCount;
    private bool isAttack;
    private bool isJump;
    private float returnStartTime = 0;
    private float hp;

    [HideInInspector] public int patternOrder;

    [Tooltip("Select Grounded Layer")]
    [SerializeField] private LayerMask groundLayer;
    [Tooltip("이동거리")]
    [SerializeField] private float p1Dis;
    [Tooltip("점프높이")]
    [SerializeField] private float p1JumpHeight;
    [Tooltip("쿨타임")]
    [SerializeField] private float p1Cooltime;
    [Tooltip("데미지")]
    [SerializeField] private float p1Dmg;
    [Tooltip("공격 횟수")]
    [SerializeField] private int p1AtkCount;
    [Tooltip("리턴 속도")]
    [SerializeField] private float p1ReturnSpd;
    [Tooltip("경직 시간")]
    [SerializeField] private float p1StunTime;
    [Space]
    [Header("이펙트")]
    [SerializeField] private GameObject moveEffect;
    [SerializeField] private GameObject dropEffect;
    private Animator hammerAnim;
    [Space]
    [SerializeField] private AudioClip dropSound;

    private void Awake()
    {
        curCooltime = p1Cooltime;
        curAttackCount = p1AtkCount;
    }

    private void Start()
    {
        playerPos = GameObject.Find("Player").transform.position;
        rb = GetComponent<Rigidbody2D>();
        col = GetComponent<BoxCollider2D>();
        childCol = transform.GetChild(0).GetComponent<BoxCollider2D>();
        ham = GetComponent<Hammer>();
        hammerAnim = GetComponent<Animator>();
        audioSource = GetComponent<AudioSource>();

        // 초기 설정
        isGrounded = true;
        patternOrder = 0;
        curCooltime = p1Cooltime;
        childCol.enabled = true;
        curAttackCount = p1AtkCount;
        moveEffect.SetActive(false);
        dropEffect.SetActive(false);
        
    }

    void Update()
    {
        hp = GetComponent<Health>().value;
        hammerAnim.SetBool("IsAttack", isAttack);
        // 1. 땅 체크
        GroundCheck();
        // 2. 쿨타임 체크
        CheckCoolTime();
        // 3. 이동
        Move();
        // 4. 리턴
        ReturnPos();
    }

    // 이동
    private void Move()
    {
        if (!isReturn)
        {
            // 땅체크 && 어택카운트 1 이상
            if (isGrounded == true && p1AtkCount > 0 && p1Cooltime <= 0)
            {
                dropEffect.SetActive(false);
                rb.velocity = Vector2.zero;
                
                // 이동
                isAttack = true;
                if (hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("open") && hammerAnim.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1.0f || hammerAnim.GetCurrentAnimatorStateInfo(0).IsName("damaged"))
                {
                    Debug.Log("패턴1 Move()");
                    anim.SetTrigger("DoAttackLeft");
                    rb.AddForce(new Vector2(p1Dis, p1JumpHeight), ForceMode2D.Impulse);
                    p1AtkCount--;
                    p1Cooltime = curCooltime;
                    isGrounded = false;
                    isJump = true;
                }
            }
        }
        if (isJump && isGrounded)
        {
            dropEffect.SetActive(true);
            PlaySound(dropSound);
            GameManager.Instance.CameraShake();
            isJump = false;
        }
        // 리턴
        if (isGrounded == true && p1AtkCount <= 0 && patternOrder == 0)
        {
            isAttack = false;
            isReturn = true;
            rb.velocity = Vector2.zero;

            // 이동
            ReturnMove();
        }
    }

    // 그라운드 체크
    private void GroundCheck()
    {
        if (rb.velocity.y < 0)
        {
            // Ray 체크
            Debug.DrawRay(transform.position, Vector2.down * (col.size.y - 0.1f), Color.red);
            rayHit = Physics2D.Raycast(transform.position, Vector2.down, col.size.y - 0.1f, LayerMask.GetMask("Ground"));

            if (rayHit.collider != null)
            {
                isGrounded = true;
            }
            else
            {
                isGrounded = false;
            }
        }
    }

    // 다음 점프 쿨타임
    private void CheckCoolTime()
    {
        if (p1Cooltime > 0)
        {
            //isAttack = false;
            p1Cooltime -= Time.deltaTime;
        }
        else
        {
            p1Cooltime = 0;
        }
    }

    // 공격 후 자리 돌아가기
    private void ReturnPos()
    {
        switch (patternOrder)
        {
            case 1:
                moveEffect.SetActive(false);
                returnStartTime = 0f;
                rb.velocity = Vector2.zero;
                rb.gravityScale = 0f;
                transform.Translate(new Vector2(0, .5f));
                break;

            case 2:
                rb.velocity = Vector2.zero;
                transform.Translate(new Vector2(.5f, 0));
                break;

            case 3:
                isReturn = false;
                //rb.velocity = Vector2.zero;
                rb.gravityScale = 4.0f;
                transform.Translate(new Vector2(0, 0));
                childCol.enabled = true;
                //returnCount = 0;
                break;
        }

    }

    // 변수 초기화
    private void Init()
    {
        isReturn = false;
        isGrounded = true;
        p1AtkCount = curAttackCount;
        patternOrder = 0;
        p1Cooltime = curCooltime;
        if (childCol != null)
            childCol.enabled = true;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.CompareTag("Player") && isAttack)
        {
            collision.GetComponent<Health>().take_damage(p1Dmg, -transform.position);
        }

        if (collision.gameObject.layer == LayerMask.NameToLayer("Ground") && patternOrder >= 3)
        {
            Init();
            gameObject.GetComponent<BossPattern1>().enabled = false;
        }
    }
    
    void ReturnMove()
    {
        returnStartTime += Time.deltaTime;
        if (returnStartTime > 3.0f)
        {
            rb.gravityScale = 0.0f;
            moveEffect.SetActive(true);
            rb.AddForce(new Vector2(-p1ReturnSpd, 0), ForceMode2D.Impulse);
            childCol.enabled = false;
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
