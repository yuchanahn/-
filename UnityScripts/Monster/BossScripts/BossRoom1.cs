using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossRoom1 : MonoBehaviour
{
    [Tooltip("보스 패턴 1")]
    public BossPattern1 bp1;
    
    // 충돌처리
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.name == bp1.name)
        {
            bp1.patternOrder++;
            Debug.Log(bp1.patternOrder);
        }
    }
}
