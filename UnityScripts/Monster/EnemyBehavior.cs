using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.Serialization;

public class EnemyBehavior : MonoBehaviour
{
    [Header("Enemy Config")]
    public float damage;
    public float attackDistance;
    public float moveSpeed;
    public float timer;
    public float parryCoolTime;
    public float Destroytime;

    [Header("Sound Config")]
    public AudioClip attackSound;
    public AudioClip backAttackSound;
    public AudioClip walkSound;
    public AudioClip hitSound;
    public AudioClip deadSound;

    [Header("부위파괴")]
    [Range(0.1f, 0.9f)] public float destructionRatio = 0.4f;
    public ParticleSystem destParticle;

    [Header("Extra Config")]
    public Transform leftLimit;
    public Transform rightLimit;
    [HideInInspector] public Transform target;
    [HideInInspector] public bool inRange;
    public GameObject hotZone;
    public GameObject triggerArea;
    public Collider2D hitBox;
    public bool canParry;
    [HideInInspector] public bool isParry;
    [HideInInspector] public bool doParry;
    [HideInInspector] public bool attackMode;

    [SerializeField] public bool is_parried_hit;
    [SerializeField] public float parry_time;

    private Animator anim;
    private AudioSource audioSource;
    [SerializeField] private float distance;
    private bool cooling;
    private float intTimer;
    public bool isDead;
    private float hp;
    private float maxHp;
    public bool isDest;
    private int destCount = 0;
    private bool isSoundPlay;
    [HideInInspector] public bool bPlay;
    /// <summary>
    /// false = right     true = left;
    /// </summary>
    [HideInInspector] public bool dirCheck;
    public float currentParryCool = 0;
    [SerializeField] AnimationCurve knock_back_speed;
    private Rigidbody2D rb2d;

    [SerializeField] private UnityEvent OnDead;
    public IEnumerator knock_back(int dir)
    {
        float t = 0;
        while (knock_back_speed.Evaluate(t) > 0)
        {
            if (isDead)
            {
                rb2d.velocity = Vector2.zero;
                yield break;
            }
            t += Time.deltaTime;
            rb2d.velocity = Vector2.right * (knock_back_speed.Evaluate(t) * dir);
            yield return new WaitForSeconds(Time.deltaTime);
        }
    }

    void Awake()
    {
        rb2d = GetComponent<Rigidbody2D>();
        SelectTarget();
        intTimer = timer;
        anim = GetComponentInChildren<Animator>();
        maxHp = GetComponent<Health>().max_value;
        audioSource = GetComponent<AudioSource>();
        //if (hitBox != null) hitBox.enabled = false;
        GetComponent<Health>().damage_ev = f =>
        {
            if (isDest && destParticle != null)
            {
                Debug.Log("부위파괴");
                if (destParticle.gameObject.activeSelf == false)
                    destParticle.gameObject.SetActive(true);
                destParticle.Play();
                destCount++;
                isDest = false;
            }

            is_parried_hit = false;
            anim.speed = 1;
        };
        GetComponent<Health>().damage_ev_with_hitpoint = (float _, Vector2 hit_pos) =>
        {
            PlaySound(hitSound);
            StartCoroutine(knock_back(hit_pos.x > transform.position.x ? -1 : 1));
        };
    }

    public void OnParriedHit()
    {
        StartCoroutine(Rigid());
    }
    void Update()
    {
        hp = GetComponent<Health>().value;

        if (!bPlay)
        {
            DestPart();

            if (hp <= 0)
            {
                if (GameManager.Instance != null && !isDead)
                {
                    GameManager.Instance.enemyCount--;
                    Debug.Log("죽음");
                    OnDead?.Invoke();
                }
                isDead = true;
            }

            Dead();
            if (is_parried_hit) return;

            if (!isDead)
            {
                // Parry System --------------------
                ParryCool();
                // Parry System --------------------

                // Patrol --------------------------
                if (!attackMode && attackDistance <= distance)
                {
                    Move();
                }
                else
                {
                    anim.SetBool("IsWalk", false);
                }

                if (!insideofLimits() && !inRange && !anim.GetCurrentAnimatorStateInfo(0).IsName("Scout_Attack"))
                {
                    SelectTarget();
                }
                // Patrol --------------------------

                // Attack CoolTime -----------------
                if (anim.GetCurrentAnimatorStateInfo(0).IsName("Scout_Attack") && anim.GetCurrentAnimatorStateInfo(0).normalizedTime >= 1.0f)
                {
                    TriggerCooling();
                }
                // Attack CoolTime -----------------

                // Player Tracking -----------------
                if (inRange)
                {
                    EnemyLogic();
                }
                // Player Tracking -----------------
            }
        }
    }

    void Dead()
    {
        if (isDead)
        {
            anim.SetBool("IsDead", true);
            PlaySound(deadSound);
            Destroy(gameObject, Destroytime);
        }
    }

    // Basic Enemy AI ----------------------
    void EnemyLogic()
    {
        distance = Vector2.Distance(transform.position, target.position);

        if ((distance) > attackDistance)
        {
            StopAttack();
        }
        else if (attackDistance >= distance && !cooling)
        {
            if (target.position.x > transform.position.x && dirCheck == false)
                StartCoroutine(Attack());
            else if (target.position.x < transform.position.x && dirCheck == false)
                StartCoroutine(BackAttack());
            else if (target.position.x < transform.position.x && dirCheck == true)
                StartCoroutine(Attack());
            else if (target.position.x > transform.position.x && dirCheck == true)
                StartCoroutine(BackAttack());
        }

        if (cooling)
        {
            Cooldown();
            anim.SetBool("IsAttack", false);
            anim.SetBool("IsBackAttack", false);
        }
    }
    // Basic Enemy AI ----------------------


    IEnumerator Rigid()
    {
        is_parried_hit = true;
        anim.CrossFade("Scout_Idle", 0);
        anim.speed = 0;

        Debug.Log("Rigid");
        yield return new WaitForSeconds(parry_time);
        Debug.Log("RigidEnd");

        anim.speed = 1;
        is_parried_hit = false;
    }

    void Move()
    {
        //audioSource.loop = true;
        PlaySound(walkSound);

        anim.SetBool("IsWalk", true);
        Vector3 rotation = transform.eulerAngles;

        if (target.position.x < transform.position.x && transform.rotation.y == 0)
            Flip();
        else if (target.position.x > transform.position.x && transform.rotation.y == -180f)
            Flip();

        if (!anim.GetCurrentAnimatorStateInfo(0).IsName("Scout_Attack") && !anim.GetCurrentAnimatorStateInfo(0).IsName("Scout_BackAttack"))
        {
            Vector2 targetPosition = new Vector2(target.position.x, transform.position.y);

            transform.position = Vector2.MoveTowards(transform.position, targetPosition, moveSpeed * Time.deltaTime);
        }
    }
    // 공격중이 아닐때 앞으로 이동 -----------

    // 공격 --------------------------------
    IEnumerator Attack()
    {
        if (cooling == false)
        {
            //timer = intTimer;
            attackMode = true;

            anim.SetBool("IsWalk", false);
            anim.SetBool("IsAttack", true);
            PlaySound(attackSound);
            yield return new WaitForSeconds(0.2f);
            //hitBox.enabled = true;

            // Parrying Test ---------------------
            isParry = true;
            if (isParry && canParry)
            {
                // ...
            }
            // Parrying Test ---------------------
        }
    }
    // 공격 --------------------------------

    // 공격 --------------------------------
    IEnumerator BackAttack()
    {
        if (cooling == false)
        {
            //timer = intTimer;
            attackMode = true;
            Flip();
            anim.SetBool("IsWalk", false);
            anim.SetBool("IsAttack", false);
            anim.SetBool("IsBackAttack", true);
            PlaySound(backAttackSound);
            yield return new WaitForSeconds(0.2f);
            //hitBox.enabled = true;

            // Parrying Test ---------------------
            isParry = true;
            if (isParry == true && canParry == true)
            {
                // ...
            }
            // Parrying Test ---------------------
        }
    }
    // 공격 --------------------------------

    // Attack CoolTime ---------------------
    void Cooldown()
    {

        //hitBox.enabled = false;

        timer -= Time.deltaTime;

        if (timer <= 0 && cooling)
        {
            cooling = false;
            timer = intTimer;
        }
    }
    // Attack CoolTime ---------------------

    void StopAttack()
    {
        cooling = false;
        attackMode = false;

        //hitBox.enabled = false;

        // Parrying Test ---------------------
        isParry = false;
        // Parrying Test ---------------------

        anim.SetBool("IsAttack", false);
        anim.SetBool("IsBackAttack", false);
    }

    public void TriggerCooling()
    {
        cooling = true;
        isSoundPlay = false;
    }

    // Patrol Limit Space --------------------
    private bool insideofLimits()
    {
        return transform.position.x > leftLimit.position.x && transform.position.x < rightLimit.position.x;
    }
    // Patrol Limit Space --------------------

    // Patrol --------------------------------
    public void SelectTarget()
    {
        float distanceToLeft = Vector2.Distance(transform.position, leftLimit.position);
        float distanceToRight = Vector2.Distance(transform.position, rightLimit.position);

        if (distanceToLeft > distanceToRight)
        {
            target = leftLimit;
        }
        else
        {
            target = rightLimit;
        }

        if (!isDead)
            Flip();
    }
    // Patrol --------------------------------

    // 순찰중 좌우 반전 -----------------------
    public void Flip()
    {
        Vector3 rotation = transform.eulerAngles;
        if (transform.position.x > target.position.x)
        {
            rotation.y = 180f;
            dirCheck = true;
        }
        else
        {
            rotation.y = 0f;
            dirCheck = false;
        }

        transform.eulerAngles = rotation;
    }
    // 순찰중 좌우 반전 -----------------------

    // Parrying CoolTime ---------------------
    private void ParryCool()
    {
        if (parryCoolTime >= currentParryCool)
        {
            currentParryCool += Time.deltaTime;
        }
        else if (parryCoolTime < currentParryCool)
        {
            canParry = true;
        }
    }
    // Parrying CoolTime ---------------------

    // Destruction Part System ---------------
    private void DestPart()
    {
        if (hp < maxHp * destructionRatio && !isDest && destCount == 0)
        {
            Debug.Log("부위파괴가능");
            isDest = true;
        }
    }
    // Destruction Part System ---------------

    private void PlaySound(AudioClip clip)
    {
        if (clip != null && !audioSource.isPlaying && !isSoundPlay)
        {
            audioSource.clip = clip;
            audioSource.Play();
            Debug.Log("사운드 : " + clip.name);
            isSoundPlay = true;
        }
    }

    public void SetbPlay(bool setplay)
    {
        bPlay = setplay;
    }
}
