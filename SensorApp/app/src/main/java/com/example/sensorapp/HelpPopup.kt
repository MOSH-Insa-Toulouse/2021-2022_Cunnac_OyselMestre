package com.example.sensorapp

import android.app.Dialog
import android.os.Bundle
import android.view.Window
import android.widget.Button


class HelpPopup(private val adapter: MainActivity) : Dialog(adapter) {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.popup_help)
        //Cette pop-up sert seulement à afficher un texte d'aide. Le code est d'une trivialié déconcertante
        findViewById<Button>(R.id.dismiss_button).setOnClickListener {
            dismiss()
        }
    }
}