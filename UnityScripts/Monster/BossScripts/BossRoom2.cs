using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BossRoom2 : MonoBehaviour
{
    [Tooltip("보스 패턴 2")]
    public BossPattern2 bp2;

    // 충돌처리
    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (collision.gameObject.name == bp2.name)
        {
            if (bp2.patternOrder == 1)
            {
                bp2.patternOrder++;
            }

            if (bp2.patternOrder == 0 && !bp2.isPatternStart)
            {
                bp2.patternOrder++;
            }
            else if (bp2.patternOrder == 0 && bp2.isPatternStart)
            {
                bp2.patternOrder = 2;
            }
        }
    }
}
