package id.ac.ukdw.firebaaseiot

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.Editable
import android.text.TextWatcher
import android.util.Log
import android.view.View
import android.widget.*
import com.google.firebase.database.*
import com.google.firebase.database.ktx.database
import com.google.firebase.ktx.Firebase


class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val database = Firebase.database
        val myRef = database.getReference("/gate1/")
        val myRefJarak = database.getReference("/gate1/jarak")
        val myRefDelay = database.getReference("/gate1/delay")

        val TAG = "ReadAndWriteSnippets"
        val jarak = findViewById<TextView>(R.id.tvJarak)
        var valueJarak: String? = null
        val statusGate = findViewById<TextView>(R.id.tvStatusGate)
        var gate: String? = null
        val btnOpen = findViewById<Button>(R.id.btnOpen)
        val btnClose = findViewById<Button>(R.id.btnClose)
        val etDelay = findViewById<EditText>(R.id.etDelay)
        var delay:Int = 5

        fun toast(message: String) {
            Toast.makeText(this, message, Toast.LENGTH_LONG).show()
        }


        myRef.addValueEventListener(object: ValueEventListener {
            override fun onDataChange(snapshot: DataSnapshot) {
                valueJarak = snapshot.child("/jarak").getValue().toString()
                gate = snapshot.child("/statusGate").getValue().toString()
                jarak.setText(valueJarak)
                statusGate.setText(gate)
            }

            override fun onCancelled(error: DatabaseError) {
                Log.w(TAG, "Failed to read value.", error.toException())
            }
        })

        etDelay.addTextChangedListener(object : TextWatcher {
            override fun afterTextChanged(s: Editable?) {
                if((s.toString()).toInt()>0){
                    delay = (s.toString()).toInt()
                }
            }

            override fun beforeTextChanged(s: CharSequence?, start: Int, count: Int, after: Int) {
            }

            override fun onTextChanged(s: CharSequence?, start: Int, before: Int, count: Int) {
            }
        })

        btnOpen.setOnClickListener {
            myRefJarak.setValue(0)
            myRefDelay.setValue(delay)
            toast("gate opened for $delay seconds")
        }

        btnClose.setOnClickListener {
            myRefJarak.setValue(100000)
            myRefDelay.setValue(delay)
            toast("gate closed for $delay seconds")
        }
    }
}
