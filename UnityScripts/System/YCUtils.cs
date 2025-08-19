using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public static class YCUtils
{
    static public IEnumerator act_on_nextframe(System.Action act) {
        yield return new WaitForEndOfFrame();
        yield return new WaitForEndOfFrame();
        act();
    }
    static public IEnumerator act_on_over_time(System.Action act, float t) {
        yield return new WaitForSeconds(t);
        act();
    }
}
