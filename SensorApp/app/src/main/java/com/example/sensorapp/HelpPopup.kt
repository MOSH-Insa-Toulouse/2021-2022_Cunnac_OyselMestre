package com.example.sensorapp

import android.app.Dialog
import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.Window
import android.widget.Button
import android.widget.CalendarView
import android.widget.TextView
import android.widget.Toast
import androidx.core.content.ContextCompat.startActivity
import com.example.userapp.Bluetooth
import org.w3c.dom.Text
import java.text.SimpleDateFormat

class HelpPopup(private val adapter: MainActivity) : Dialog(adapter) {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.popup_help)

        findViewById<Button>(R.id.dismiss_button).setOnClickListener {
            dismiss()
        }
    }
    }