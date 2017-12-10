using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ThemeButton : MonoBehaviour {
    public int theme = 0;
    private ChangeTheme script;

	// Use this for initialization
	void Start () {
        script = GameObject.Find("ThemeMenu").GetComponent<ChangeTheme>();
    }

    // Update is called once per frame
    private void OnTriggerEnter(Collider collider)
    {
        Debug.Log("Theme button trigger");
        script.setTheme(theme);
    }
}
