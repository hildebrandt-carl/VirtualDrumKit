using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ChangeTheme : MonoBehaviour {
    private int theme = 0;

    private Renderer ground;
    private Renderer menu;
    private Renderer pmenu;
    private GameObject drumkit;

    public GameObject classicPrefab;
    public GameObject neonPrefab;
    public GameObject naturePrefab;

    private Theme classic;
    private Theme neon;
    private Theme nature;

    // Use this for initialization
    void Start () {
        ground = GameObject.Find("Ground").GetComponent<Renderer>();
        menu = GameObject.Find("ThemeMenu").GetComponent<Renderer>();
        pmenu = GameObject.Find("PatternMenu").GetComponent<Renderer>();

        classic = classicPrefab.GetComponent<Theme>();
        neon    = neonPrefab.GetComponent<Theme>();
        nature  = naturePrefab.GetComponent<Theme>();

        drumkit = GameObject.Find("ClassicDrumSet");
    }

    public void setTheme(int theme)
    {
        switch (theme)
        {
            case 0: //CLASSIC
                Destroy(drumkit);
                drumkit = Instantiate(classicPrefab);
                ground.material = classic.ground;
                menu.material = classic.menu;
                pmenu.material = classic.menu;
                RenderSettings.skybox = classic.skybox;
                break;
            case 1: //NEON
                Destroy(drumkit);
                drumkit = Instantiate(neonPrefab);
                ground.material = neon.ground;
                menu.material = neon.menu;
                pmenu.material = neon.menu;
                RenderSettings.skybox = neon.skybox;
                break;
            case 2: //NATURE
                Destroy(drumkit);
                drumkit = Instantiate(naturePrefab);
                ground.material = nature.ground;
                menu.material = nature.menu;
                pmenu.material = nature.menu;
                RenderSettings.skybox = nature.skybox;
                break;
        }
    }
}
