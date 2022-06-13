

function myFunction(){

    db.collection("readings").get().then(readings => {
        console.log(readings.size);
        var count = 0;
        readings.forEach((doc) => {
            count++;
            if(count === readings.size){
                console.log(`${doc.id} => ${doc.data()}`);
                console.log(doc.data());
                document.getElementById("date").innerHTML = `Date : ${doc.data().date}`;
                document.getElementById("time").innerHTML = `Time : ${doc.data().time}`;
                var moisture = (parseInt(doc.data().soil_moisture) / 860) *100;
                document.getElementById("moisture").innerHTML = `Moisture Reading : ${moisture.toFixed(2)}%`;
                document.getElementById("temp").innerHTML = `Temperature Reading : ${doc.data().temp} &#8451`;
                document.getElementById("humidity").innerHTML = `Humidity Reading : ${doc.data().humidity}%`;
            }
        });
    });
}