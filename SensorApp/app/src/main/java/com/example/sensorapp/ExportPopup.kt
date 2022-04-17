package com.example.sensorapp

import android.app.Dialog
import android.content.ActivityNotFoundException
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.util.Log
import android.view.Window
import android.widget.Button
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.content.ContextCompat.startActivity
import com.anychart.chart.common.dataentry.DataEntry

class ExportPopup(private val adapter: MainActivity, val data: MutableList<DataEntry>) : Dialog(adapter) {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestWindowFeature(Window.FEATURE_NO_TITLE)
        setContentView(R.layout.popup_export)

        findViewById<Button>(R.id.other_button).setOnClickListener {
            other_data_send()
        }

        findViewById<Button>(R.id.email_button).setOnClickListener {
            email_data_send()
        }
    }

    fun other_data_send() {

        Log.i(TAG, "data send FUNCTION")

        if (data_array.isEmpty()) Toast.makeText(adapter, "No data to export !", Toast.LENGTH_SHORT).show()
        else {
            val sendIntent = Intent(Intent.ACTION_VIEW)

            var stringToSend = ""

            for (data_element in data_array){ //Make text to send
                stringToSend += "$data_element\n"
            }
            Log.i(TAG, "Data to send: $stringToSend")

            sendIntent.putExtra(Intent.EXTRA_TEXT, stringToSend) //text to write

            try {
                startActivity(adapter, Intent.createChooser(sendIntent, "Send data..."), null)
                Log.i(TAG, "BONJOUR")
            } catch (ex: ActivityNotFoundException) {
                Toast.makeText(
                    adapter,
                    "Error, oups...", Toast.LENGTH_SHORT
                ).show()
            }
        }
    }

    fun email_data_send() {

        Log.i(TAG, "email send FUNCTION")

        if (data_array.isEmpty()) Toast.makeText(adapter, "No data to export !", Toast.LENGTH_SHORT).show()
        else {
            val sendIntent = Intent(Intent.ACTION_SENDTO)

            var stringToSend = ""

            for (data_element in data_array){ //Make text to send
                stringToSend += "$data_element\n"
            }
            Log.i(TAG, "Data to send: $stringToSend")

            sendIntent.putExtra(Intent.EXTRA_TEXT, stringToSend) //text to write

            val emailSubject = "MOSH Sensor data"

            sendIntent.putExtra(Intent.EXTRA_SUBJECT, emailSubject) //put email subject in intent

            sendIntent.type = "text/plain"

            sendIntent.data = Uri.parse("mailto:") // allows to chose other email apps
            try {
                startActivity(adapter, Intent.createChooser(sendIntent, "Send email..."), null)
                Log.i(TAG, "BONJOUR")
            } catch (ex: ActivityNotFoundException) {
                Toast.makeText(
                    adapter,
                    "Error, oups...", Toast.LENGTH_SHORT
                ).show()
            }
        }
    }
}