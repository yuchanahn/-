using System;
using System.Collections;
using System.Linq;
using UnityEngine;
using UnityEngine.Events;
using pc_st = Fsm.State<pc_data_t>;
using Random = System.Random;

class pc_data_t
{
    public Animator amt;
    public ChargingAttack cg_atk;
    public PC pc;
}

#region PC_FSM

class move_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("walk");

        data.pc.audio_binder.Play("PC_Step1", eAudioType.SFX, true);

        base.OnEnter(data);
    }

    public override void OnFixedUpdate(pc_data_t data)
    {
        data.pc.apply_velocity_move();
        base.OnFixedUpdate(data);
    }

    public override void OnUpdate(pc_data_t data)
    {
    }

    public override void OnExit(pc_data_t data)
    {
        data.pc.audio_binder.Stop(eAudioType.SFX);
        base.OnExit(data);
    }
}

class attack_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.pc.atk_move_x = data.pc.in_move_x;
        data.pc.combo_time = 0;
        data.amt.Play($"attack{data.pc.combo_count % 3 + 1 + data.pc.StartComboCount}");
        data.pc.audio_binder.PlayOneShot($"PC_Atk{data.pc.combo_count % 3 + 1}", eAudioType.SFX);
        
        data.pc.AtkEvent?.Invoke();
        if(data.pc.combo_count == 2) data.pc.ThreeComboEvent?.Invoke();
        
        data.pc.combo_count++;
        data.pc.rb.velocity = Vector2.up * data.pc.rb.velocity.y;
        data.pc.GetComponent<ComboAttack>().combo_attack(data.pc.combo_count + data.pc.StartComboCount);

        base.OnEnter(data);
    }


    public override void OnFixedUpdate(pc_data_t data)
    {
        data.pc.apply_velocity_move();
        base.OnFixedUpdate(data);
    }


    public override void OnExit(pc_data_t data)
    {
        data.pc.combo_time = 0;
        base.OnExit(data);
    }
}

class charge_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("charge");
        data.pc.rb.velocity = Vector2.up * data.pc.rb.velocity.y;
        
        base.OnEnter(data);
    }

    public override void OnExit(pc_data_t data)
    {
        data.cg_atk.end_charge();
        data.pc.ChargeAtkFirstEnd();
        base.OnExit(data);
    }
}

class jump_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("jump");
        data.pc.apply_velocity_jump();

        base.OnEnter(data);
    }

    public override void OnFixedUpdate(pc_data_t data)
    {
        data.pc.apply_velocity_move();
    }
}

class idle_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("idle");
        base.OnEnter(data);
    }

    public override void OnUpdate(pc_data_t data)
    {
        data.pc.apply_velocity_move();
    }
}

class charge_atk_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("charge_attack");
        data.pc.ChargeAtkEvent?.Invoke();
        base.OnEnter(data);
    }
}

class fall_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("fall");
        base.OnEnter(data);
    }

    public override void OnFixedUpdate(pc_data_t data)
    {
        data.pc.apply_velocity_move();
    }
}

class defence_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("defence");
        data.pc.GetComponent<Health>().damage_func = damage =>
        {
            data.pc.play_defence_effect();
            Debug.Log("defence!");
            return damage * data.pc.defence_damage;
        };
        
        data.pc.DefenceEvent?.Invoke();
        
        base.OnEnter(data);
    }

    public override void OnFixedUpdate(pc_data_t data)
    {
        data.pc.in_move_x = 0;
        data.pc.apply_velocity_move();
        base.OnFixedUpdate(data);
    }

    public override void OnExit(pc_data_t data)
    {
        data.pc.GetComponent<Health>().damage_func = null;
        base.OnExit(data);
    }
}

class roll_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        if (data.pc.GetComponent<Stamina>().value >= data.pc.roll_stamina)
        {
            data.pc.GetComponent<Stamina>().value -= data.pc.roll_stamina;
        }
        else
        {
            data.pc.is_roll = false;
            return;
        }

        data.amt.Play("roll");
        data.pc.audio_binder.PlayOneShot("PC_Roll", eAudioType.SFX);

        data.pc.GetComponent<Health>().damage_func = _ => 0;

        base.OnEnter(data);
    }

    public override void OnFixedUpdate(pc_data_t data)
    {
        data.pc.apply_velocity_move();
    }

    public override void OnExit(pc_data_t data)
    {
        data.pc.GetComponent<Health>().damage_func = null;
        base.OnExit(data);
    }
}

class hit_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.pc.GetComponent<AnimEvBind>().ani_event("ATK_COL1OFF");
        data.pc.GetComponent<AnimEvBind>().ani_event("ATKEND");
        data.amt.Play("hit");
        data.pc.audio_binder.SetPlaySpeed(2.5f, eAudioType.SFX);
        data.pc.audio_binder.PlayOneShot("PC_Hit", eAudioType.SFX);
        data.pc.is_attack = false;
        data.pc.combo_count = 0;

        base.OnEnter(data);
    }

    public override void OnExit(pc_data_t data)
    {
        data.pc.audio_binder.SetPlaySpeed(1.0f, eAudioType.SFX);
        base.OnExit(data);
    }
}

class die_t : pc_st
{
    public override void OnEnter(pc_data_t data)
    {
        data.amt.Play("die");
        //data.pc.audio_binder.PlayOneShot("PC_Die", eAudioType.SFX);
        //data.pc.audio_binder.Stop(eAudioType.BGM);
        data.pc.GetComponent<Health>().damage_func = null;
        base.OnEnter(data);
    }

    public override void OnExit(pc_data_t data)
    {
        data.pc.GetComponent<Health>().damage_func = null;
        base.OnExit(data);
    }
}

#endregion

public class PC : MonoBehaviour
{
    #region PC_Data

    [Header("Movement")] [SerializeField, Range(0, 40)]
    public float max_speed = 3;

    [SerializeField, Range(0, 40)] float jump_speed = 9;
    [Space(10f)] [SerializeField] public bool is_grounded = true;
    [SerializeField] LayerMask grd_ly;
    [Header("Input")] [SerializeField] public float in_move_x;
    [SerializeField] public bool jump_start = false;
    [SerializeField] public bool is_attack = false;
    [SerializeField] public bool is_attack_start = false;
    [SerializeField] public bool is_charging_attack = false;
    [SerializeField] public bool is_roll = false;
    [HideInInspector] public Rigidbody2D rb;
    [Header("Attack")] [SerializeField] public float normal_atk_limit_time = 0.1f;
    [SerializeField] public float atk_stamina;
    [SerializeField] public float charge_atk_stamina;
    [SerializeField] public float attack_move_speed = 0.5f;
    [Header("Roll")] [SerializeField] public float roll_stamina;

    [Header("Defence")] [SerializeField, Range(0f, 1f)]
    public float defence_damage = 1f;

    [SerializeField] public bool is_defance = false;
    [Header("Hit")] [SerializeField] public bool is_hit = false;

    #endregion

    public AudioBinder audio_binder;
    DJTimer normal_attack_timer;
    DJTimer doublebutton_timer;
    YCInputSystem input;
    [SerializeField] Animator SlashAmt;
    public bool is_die = false;


    [SerializeField] public int StartComboCount;


    [SerializeField] public float combo_timelimit = 0.5f;
    [SerializeField] public float combo_time = 0.0f;
    [SerializeField] public int combo_count = 0;
    
    public UnityEvent ThreeComboEvent;
    public UnityEvent AtkEvent;
    public UnityEvent ChargeAtkEvent;
    public UnityEvent DefenceEvent;
    public UnityEvent MoveEvent;
    public UnityEvent MoveREvent;
    public UnityEvent MoveLEvent;
    public UnityEvent JumpEvent;
    
    void Start()
    {
        health = GetComponent<Health>();
        rb = GetComponent<Rigidbody2D>();

        var anim_event = GetComponent<AnimEvBind>();
        anim_event.binds.First(x => x.name == "ATKEND").act = () => { is_attack = false; };

        anim_event.binds.First(x => x.name == "HITEND").act = () => { is_hit = false; };

        anim_event.binds.First(x => x.name == "ATK_MOVE").act = () => { StartCoroutine(skill_move(atk_move_speed)); };
        anim_event.binds.First(x => x.name == "ATK_MOVE2").act = () => { StartCoroutine(skill_move(atk_move_speed, 2.5f)); };
        anim_event.binds.First(x => x.name == "CATK_MOVE").act = () =>
        {
            StartCoroutine(skill_jump(charge_atk_jump_curve));
            StartCoroutine(skill_move(charge_atk_speed));
        };

        anim_event.binds.First(x => x.name == "ROLL_MOVE").act = () => { StartCoroutine(roll_move(1f)); };

        anim_event.binds.First(x => x.name == "ROLLEND").act = () => { is_roll = false; };

        anim_event.binds.First(x => x.name == "DIEEND").act = () =>
        {
            is_die = false;
            UnityEngine.SceneManagement.SceneManager.LoadScene("GameOver");
        };
        anim_event.binds.First(x => x.name == "START_SLASH").act = () =>
        {
            SlashAmt.gameObject.SetActive(true);
            SlashAmt.CrossFade("Slash01", 0f);
        };

        anim_event.binds.First(x => x.name == "END_SLASH").act = () => { SlashAmt.gameObject.SetActive(false); };
    }

    public void SpriteAnimStart(string animName)
    {
        SlashAmt.gameObject.SetActive(true);
        SlashAmt.CrossFade(animName, 0f);
    }

    public void SpriteAnimEnd()
    {
        SlashAmt.gameObject.SetActive(false);
    }


    public void MovementControl(int dir)
    {
        in_move_x = dir;
    }
    
    public float atk_move_x = 0f;
    public float cur_dir => sr.transform.rotation.eulerAngles.y != 0 ? -1 : 1;
    [SerializeField] AnimationCurve atk_move_speed;
    [SerializeField] AnimationCurve roll_speed;
    [SerializeField] AnimationCurve knock_back_speed;
    [SerializeField] AnimationCurve charge_atk_speed;
    [SerializeField] AnimationCurve charge_atk_jump_curve;

    [SerializeField] GameObject parry_col;
    [SerializeField] float parry_time = 0.5f;
    [SerializeField] private Transform defence_pos;
    public void play_defence_effect()
    {
        var scl = 1.0f;
        var eft = ObjPool.take("defence_eft", 0.5f).GetComponent<SpriteEffectController>();
        eft.transform.position = defence_pos.position + (Vector3)(UnityEngine.Random.insideUnitCircle * scl);
        eft.OnAniEnd = () => eft.gameObject.SetActive(false);
    }
    
    IEnumerator skill_move(AnimationCurve ac, float val = 1.0f)
    {
        float t = 0;
        while (ac.Evaluate(t) > 0)
        {
            var pos = transform.position;
            t += Time.fixedDeltaTime;
            pos.x += ac.Evaluate(t) * val * cur_dir;
            transform.position = pos;
            yield return new WaitForSeconds(Time.fixedDeltaTime);
        }
    }
    IEnumerator skill_jump(AnimationCurve ac, float val = 1.0f)
    {
        float t = 0;
        while (ac.Evaluate(t) > 0)
        {
            var pos = transform.position;
            t += Time.fixedDeltaTime;
            pos.y += ac.Evaluate(t) * val;
            transform.position = pos;
            yield return new WaitForSeconds(Time.fixedDeltaTime);
        }
    }
    IEnumerator roll_move(float val = 1.0f)
    {
        float t = 0;
        while (roll_speed.Evaluate(t) > 0)
        {
            var pos = transform.position;
            t += Time.fixedDeltaTime;
            pos.x += roll_speed.Evaluate(t) * val * cur_dir;
            transform.position = pos;
            yield return new WaitForSeconds(Time.fixedDeltaTime);
        }
    }

    public IEnumerator knock_back(int dir)
    {
        float t = 0;
        while (knock_back_speed.Evaluate(t) > 0)
        {
            var pos = transform.position;
            t += Time.fixedDeltaTime;
            pos.x += knock_back_speed.Evaluate(t) * dir;
            transform.position = pos;
            yield return new WaitForSeconds(Time.fixedDeltaTime);
        }
    }

    Fsm.Fsm<pc_data_t> create_fsm()
    {
        var idle_st = new idle_t();
        var move_st = new move_t();
        var attack_st = new attack_t();
        var charge_st = new charge_t();
        var jump_st = new jump_t();
        var charge_atk_st = new charge_atk_t();
        var fall_st = new fall_t();
        var roll_st = new roll_t();
        var defence_st = new defence_t();
        var hit_st = new hit_t();
        var die_st = new die_t();

        var normal_fw = new Fsm.Flow<pc_data_t>();

        normal_fw
            .Do("idle", _ => idle_st, _ =>
            {
                if (is_die) return "die";
                if (is_hit) return "hit";
                if (is_attack) return "attack";
                if (GetComponent<ChargingAttack>().IsCharging) return "charge";
                if (jump_start && is_grounded)
                {
                    jump_start = false;
                    apply_velocity_jump();
                    return "jump";
                }
                
                if (in_move_x != 0) return "move";
                if (is_defance) return "defence";

                return null;
            })
            .Do(name: "move", _ => move_st, _ =>
            {
                if (is_die) return "die";
                if (is_hit) return "hit";
                if (is_attack) return "attack";
                if (is_defance) return "defence";
                if (GetComponent<ChargingAttack>().IsCharging) return "charge";
                if (is_roll) return "roll";
                if (jump_start && is_grounded)
                {
                    jump_start = false;
                    apply_velocity_jump();
                    return "jump";
                }

                ;
                if (in_move_x == 0) return "idle";

                return null;
            })
            .Do("attack", _ => attack_st, _ =>
            {
                if (is_die) return "die";
                if (!is_attack) return "idle";
                if (is_hit) return "hit";
                return null;
            })
            .Do("charge", _ => charge_st, _ =>
            {
                if (is_charging_attack) return "charge_atk";
                if (!GetComponent<ChargingAttack>().IsCharging) return "idle";

                return null;
            })
            .Do("jump", _ => jump_st, _ =>
            {
                if (is_die) return "die";
                if (rb.velocity.y <= 0) return "fall";
                return null;
            })
            .Do("charge_atk", _ => charge_atk_st, _ =>
            {
                if (!is_charging_attack) return "idle";
                return null;
            })
            .Do("fall", _ => fall_st, _ =>
            {
                if (is_die) return "die";
                if (is_grounded) return "idle";
                return null;
            })
            .Do("roll", _ => roll_st, _ =>
            {
                if (!is_roll) return "move";
                return null;
            })
            .Do("defence", _ => defence_st, _ =>
            {
                if (is_die) return "die";
                if (!is_defance) return "idle";
                return null;
            })
            .Do("hit", _ => hit_st, _ =>
            {
                if (is_die) return "die";
                if (!is_hit) return "idle";
                return null;
            }).Do("die", _ => die_st, _ => !is_die ? "idle" : null)
            ;

        pc_data_t data = new pc_data_t();
        return new Fsm.Fsm<pc_data_t>(data, normal_fw);
    }

    Fsm.Fsm<pc_data_t> pc_fsm;

    private SpriteRenderer sr;
    private BoxCollider2D boxCollider2D;
    private Health health;
    private ChargingAttack chargingAttack;

    public void ChargeAtkFirstEnd()
    {
        pc_fsm.data.amt.Play("charge_loop");
    }

    public void ChargeAtkSecondEnd()
    {
        is_charging_attack = false;
    }
    
    public void FlipSlash(int dir)
    {
        var slashAmtTransform = SlashAmt.transform;
        var scale = slashAmtTransform.localScale;
        scale.x = dir * 3;
        slashAmtTransform.localScale = scale;
    }
    
    bool tut_left = false;
    bool tut_right = false;
    
    void Awake()
    {
        chargingAttack = GetComponent<ChargingAttack>();
        boxCollider2D = GetComponent<BoxCollider2D>();
        sr = GetComponentInChildren<SpriteRenderer>();
        input = GetComponent<YCInputSystem>();
        Stamina stamina = GetComponent<Stamina>();

        doublebutton_timer = new DJTimer(0.5f);
        normal_attack_timer = new DJTimer(normal_atk_limit_time);

        pc_fsm = create_fsm();
        pc_fsm.data.pc = this;
        pc_fsm.data.amt = GetComponent<Animator>();
        pc_fsm.data.cg_atk = GetComponent<ChargingAttack>();

        GetComponent<Health>().dead_ev = () => { is_die = true; };
        GetComponent<Health>().damage_ev = _ => { is_hit = true; };
        GetComponent<Health>().damage_ev_with_hitpoint = (x, hit_pos) =>
        {
            if (is_defance)
            {
                var e = ObjPool.take("hit_effect", 0.3f);
                var position = transform.position;
                position += Vector3.up * 4;
                e.transform.position = position;
                e.transform.rotation = Quaternion.Euler(0, 0, 0);
            }
            else
            {
                StartCoroutine(knock_back(hit_pos.x - transform.position.x > 0 ? 1 : -1));
            }
        };

         
        input.axis1d.get_event_of_name("Move").ev = x =>
        {
            if (!is_grounded) return;
            in_move_x = is_attack ? (Math.Abs(in_move_x - atk_move_x) < 0.1f ? atk_move_x : 0) : x;
            if (Math.Abs(x - 1) < 0.1f) {
                tut_right = true;
                MoveREvent?.Invoke();
            } else if(Math.Abs(x - (-1)) < 0.1f) {
                tut_left = true;
                MoveLEvent?.Invoke();
            } if (tut_left && tut_right) {
                MoveEvent?.Invoke();
            }
        };

        input.key.get_event_of_name("Defence").press_ev = x =>
        {
            is_defance = true;
            if (GetComponent<Stamina>().value == 0) return;
            parry_col.SetActive(true);
            parry_col.GetComponent<PlayerHitHandler>().ColStart();
            StartCoroutine(YCUtils.act_on_over_time(() =>
            {
                parry_col.SetActive(false);
                parry_col.GetComponent<PlayerHitHandler>().ColEnd();
            }, parry_time));
        };
        input.key.get_event_of_name("Defence").unpress_ev = x => { is_defance = false; };

        input.key.get_event_of_name("Jump").press_ev = x =>
        {
            if (!is_grounded) return;
            jump_start = x;
            JumpEvent?.Invoke();
        };

        input.key.get_event_of_name("Roll").press_ev = x =>
        {
            if ((in_move_x != 0) && !is_roll) is_roll = true;
        };
        input.key.get_event_of_name("Roll").unpress_ev = x => { };


        input.key.get_event_of_name("Attack").press_ev = x =>
        {
            if (!is_grounded) return;

            is_attack_start = true;
            normal_attack_timer.Reset();
        };
        input.key.get_event_of_name("Attack").unpress_ev = x =>
        {
            if (!is_grounded || is_attack)
            {
                is_attack_start = false;
                return;
            }

            if (is_attack_start)
            {
                is_attack_start = false;
                if (!(stamina.value > atk_stamina)) return;
                is_attack = true;
                normal_attack_timer.Reset();
                stamina.value -= atk_stamina;
            }
            else if (chargingAttack.IsCharging && chargingAttack.IsChargeAttackReady)
            {
                chargingAttack.end_charge();
                if (!(stamina.value > charge_atk_stamina)) return;
                stamina.value -= charge_atk_stamina;
                is_charging_attack = true;
            }
            else
            {
                chargingAttack.end_charge();
            }
        };
    }

    void Update()
    {
        if (is_attack_start) normal_attack_timer.Tick();
        if (!is_grounded) {
            normal_attack_timer.Reset();
            is_attack_start = false;
        }

        doublebutton_timer.Tick();

        if (normal_attack_timer.IsEnded) {
            chargingAttack.start_charge();
            is_attack = false;
            is_attack_start = false;
            normal_attack_timer.Reset();
        }

        combo_time += Time.deltaTime * (is_attack ? 0f : 1f);
        if (combo_time > combo_timelimit) {
            combo_count = 0;
        }

        pc_fsm.UpdateFsm();
        pc_fsm.Update();
        jump_start = false;

        if (health.value <= 0) {
            //UnityEngine.SceneManagement.SceneManager.LoadScene("GameOver");
        }
    }

    public void apply_velocity_jump()
    {
        Vector2 v = rb.velocity;
        v.y = jump_speed * 1;
        rb.velocity = v;
    }

    public void apply_velocity_move()
    {
        Vector2 v = rb.velocity;
        v.x = in_move_x * (is_attack ? attack_move_speed : max_speed);
        
        rb.velocity = v;
    }

    public bool is_moveable = true;
    [SerializeField] float fall_start_y = -1;
    private void FixedUpdate()
    {
        if (!is_moveable) return;
        var xy = boxCollider2D.size;
        xy.x *= 0.99f;

        bool gr = is_grounded;
        is_grounded = Physics2D.OverlapBoxAll(rb.position + (Vector2.down * -2.9f), xy, 0, grd_ly).Length > 0;

        switch (gr)
        {
            case true when !is_grounded:
                Debug.Log("fall");
                fall_start_y = transform.position.y;
                break;
            case false when is_grounded:
            {
                if (fall_start_y - transform.position.y > 15)
                {
                    health.take_damage(10);
                }

                fall_start_y = transform.position.y;
                break;
            }
        }
        
        
        #region Draw Ground Check

        //var pos = rb.position + (Vector2.down * -2.9f);
        //Debug.DrawLine(pos + new Vector2(xy.x / 2, xy.y), pos + new Vector2(xy.x / 2, -xy.y / 2), Color.red);
        //Debug.DrawLine(pos + new Vector2(-xy.x / 2, xy.y), pos + new Vector2(-xy.x / 2, -xy.y / 2), Color.red);

        #endregion

        if (rb.velocity.y > 0.001f)
        {
            fall_start_y = transform.position.y;
            is_grounded = false;
        }

        var x = rb.velocity.x;

        if (x is > -0.001f and < 0.001f) x = 0;

        var rot = sr.transform.rotation;
        rot.y = x < 0 ? 180f : 0f;
        if (x != 0)
        {
            sr.transform.rotation = rot;
            FlipSlash(Math.Abs(rot.y - 180f) < 0.01f ? -1 : 1);
        }

        pc_fsm.FixedUpdate();
    }

    public void OnParrySuccess()
    {
        GetComponent<Stamina>().value = 0;
    }
}