using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

enum BOSSSTATE
{
    PATTERN1,
    PATTERN2,
    PATTERN3,
    PATTERN4,
    PATTERN5,
}

public class BossState : MonoBehaviour
{
    [Header("패턴 스크립트 할당")]
    [SerializeField] private BossPattern1 bp1;
    [SerializeField] private BossPattern2 bp2;
    [SerializeField] private BossPattern3[] bp3;
    [SerializeField] private BossPattern4 bp4;
    [SerializeField] private BossPattern5 bp5;

    [SerializeField] private BOSSSTATE state;
    [SerializeField] private float waitForNextPattern;

    private Boss boss;
    private float timer;
    private float timer5 = 0;
    [SerializeField] private float pattern5Timer = 60;
    [HideInInspector] public int destroyCount = 0;
    public bool startPattern5;
    public bool isPattern;


    private void Start()
    {
        Init();
        boss = GetComponentInParent<Boss>();
    }

    void Update()
    {
        if (!boss.isDead)
            RunState();
    }

    void Init()
    {
        bp1.enabled = false;
        bp2.enabled = false;
        for (int i = 0; i < bp3.Length; i++)
        {
            bp3[i].enabled = false;
        }
        bp4.gameObject.SetActive(false);
    }

    void RunState()
    {
        timer5 += Time.deltaTime;
        
        if(boss.deadCount < 3 && timer5 >= pattern5Timer)
        {
            startPattern5 = true;
        }

        //CheckPlayPattern();
        if (bp1.enabled == false && bp2.enabled == false && bp3[0].enabled == false && bp3[1].enabled == false && bp4.gameObject.activeSelf == false)
        {
            isPattern = false;
            timer += Time.deltaTime;
            if (timer > waitForNextPattern)
            {
                timer = 0;
                ChangeState();
                SetState();
            }
        }
    }

    void ChangeState()
    {
        if (!isPattern)
        {
            // 랜덤 스테이트 설정
            int rand = UnityEngine.Random.Range(0, 4);
            while (state == (BOSSSTATE)rand)
            {
                rand = UnityEngine.Random.Range(0, 4);
            }
            state = (BOSSSTATE)rand;
            Debug.Log("SetState()");
            isPattern = true;
        }
    }

    void SetState()
    {
        if (!startPattern5 && isPattern)
        {
            switch (state)
            {
                case BOSSSTATE.PATTERN1:
                    bp1.enabled = true;
                    bp2.enabled = false;
                    for (int i = 0; i < bp3.Length; i++)
                    {
                        bp3[i].enabled = false;
                    }
                    for (int i = 0; i < bp4.col.Length; i++)
                    {
                        bp4.col[i].enabled = false;
                    }
                    bp4.gameObject.SetActive(false);
                    break;
                case BOSSSTATE.PATTERN2:
                    bp1.enabled = false;
                    bp2.enabled = true;
                    for (int i = 0; i < bp3.Length; i++)
                    {
                        bp3[i].enabled = false;
                    }
                    for (int i = 0; i < bp4.col.Length; i++)
                    {
                        bp4.col[i].enabled = false;
                    }
                    bp4.gameObject.SetActive(false);
                    break;
                case BOSSSTATE.PATTERN3:
                    bp1.enabled = false;
                    bp2.enabled = false;
                    for (int i = 0; i < bp3.Length; i++)
                    {
                        bp3[i].enabled = true;
                    }
                    for (int i = 0; i < bp4.col.Length; i++)
                    {
                        bp4.col[i].enabled = false;
                    }
                    bp4.gameObject.SetActive(false);
                    break;
                case BOSSSTATE.PATTERN4:
                    bp1.enabled = false;
                    bp2.enabled = false;
                    for (int i = 0; i < bp3.Length; i++)
                    {
                        bp3[i].enabled = false;
                    }

                    bp4.gameObject.SetActive(true);
                    for (int i = 0; i < bp4.col.Length; i++)
                    {
                        bp4.col[i].enabled = true;
                    }
                    break;

            }
        }
        else if(startPattern5 && isPattern)
        {
            state = BOSSSTATE.PATTERN5;
            bp1.enabled = false;
            bp2.enabled = false;
            for (int i = 0; i < bp3.Length; i++)
            {
                bp3[i].enabled = false;
            }
            for (int i = 0; i < bp4.col.Length; i++)
            {
                bp4.col[i].enabled = false;
            }
            bp4.gameObject.SetActive(false);
            bp5.gameObject.SetActive(true);
        }
    }
}