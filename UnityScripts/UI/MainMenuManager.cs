using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class MainMenuManager : MonoBehaviour
{


    [Header("버튼 할당")]
    public Button newStartBtn;
    public Button loadGameBtn;
    public Button optionBtn;
    public Button exitBtn;

    [Header("팝업 리소스 할당")]
    public GameObject optionPanel;
    public GameObject popupImg;
    public Button newGameYesBtn;
    public Button newGameNoBtn;

    [Header("옵션 버튼 할당")]
    public Button backToMenu;

    [Header("그외")]
    public UIView mainUIView;
    public UIView newGameUIView;
    public UIView optionUIView;

    void Start()
    {
        var sl = FindObjectOfType<SaveLoad>();
        // 새 게임 시작
        if (newStartBtn != null)
        {
            newStartBtn.onClick.AddListener(() =>
            {
                //popupImg.SetActive(true);
                //mainUIView.enabled = false;
                //newGameUIView.enabled = true;
                sl.Save(new save_ingame_data_t
                {
                    hp = 100,
                    sp = 100,
                    potion_count = 3,
                    IsNewGame = true,
                });
                UnityEngine.SceneManagement.SceneManager.LoadScene("LoadingScene");
            });
        }
        // 게임 불러오기
        if (loadGameBtn != null)
        {
            var d = sl.Load<save_ingame_data_t>();
            var isNotLoadable = !d.HasValue || d.Value.IsNewGame || d.Value.reset;

            if (isNotLoadable)
            {
                loadGameBtn.GetComponentInChildren<TextMeshProUGUI>().color = Color.gray;
            }
            else
            {
                loadGameBtn.onClick.AddListener(() =>
                {
                    var newData = d.Value;
                    newData.scene_change = false;
                    sl.Save(newData);
                    UnityEngine.SceneManagement.SceneManager.LoadScene("LoadingScene");
                });
            }
            

        }
        // 옵션
        if (optionBtn != null)
        {
            optionBtn.onClick.AddListener(() =>
            {
                //optionPanel.SetActive(true);
                //mainUIView.enabled = false;
                //optionUIView.enabled = true;
            });
        }
        // 게임종료
        if (exitBtn != null)
        {
            exitBtn.onClick.AddListener(() =>
            {
#if UNITY_EDITOR
                UnityEditor.EditorApplication.isPlaying = false;
#else
                Application.Quit();
#endif
            });
        }

        if (newGameYesBtn != null)
        {
            newGameYesBtn.onClick.AddListener(() => { UnityEngine.SceneManagement.SceneManager.LoadScene("LoadingScene"); });
        }
        if (newGameNoBtn != null)
        {
            newGameNoBtn.onClick.AddListener(() =>
            {
                popupImg.SetActive(false);
                newGameUIView.enabled = false;
                mainUIView.enabled = true;
            });
        }

        if (backToMenu != null)
        {
            backToMenu.onClick.AddListener(() => 
            {
                optionPanel.SetActive(false);
                optionUIView.enabled = false;
                mainUIView.enabled = true;
            });
        }
    }
}
