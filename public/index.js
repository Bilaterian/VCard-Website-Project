// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            stuff: "Value 1",
            junk: "Value 2"
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
            $('#blah').html("On page load, received string '"+data.stuff+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
	
	let __dirname = window.location.pathname;
	
	$('#uploadBtn').click(function(e){
		let filename = $('#file').val().split('\\').pop();
        e.preventDefault();
		console.log("upload initiated");
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json',       //Data type - we will use JSON for almost everything 
			url: '/library',   //The server endpoint we are connecting to
			data: {
				Dfilename: filename,
				Dname: "",
				Dprops: 0,
			},
			success: function (data) {
				if(data.Dfilename == "null"){
					console.log("unsuccessful exit");
					
				}
				else{
					console.log("successful transfer");
					$('#tBody').append("<tr>"+
					"<td>"+"<a href="+'"'+"/uploads/"+data.Dfilename+'"'+">"+data.Dfilename+""+"</td>"+
					"<td>"+data.Dname+"</td>"+
					"<td>"+data.Dprops+"</td>"
					+"</tr>");
					$('#links').append("<button id="+'"view'+count+'"'+"onclick=loadTable('"+data.Dfilename+"')>"+data.Dfilename+"</button><br>");
					count = count + 1;
				}
			}
		});
	});
	let count = 1;
	
	$('#newCardBtn').click(function(e){
		e.preventDefault();
		let card = '{"FN":"';
		let name = document.getElementById('entryBoxName').value;
		card = card + name;
		card = card + '"}'
		name = name + ".vcf";
		$.ajax({
			type: 'get',            //Request type
			dataType: 'json',       //Data type - we will use JSON for almost everything 
			url: '/uploadNew',   //The server endpoint we are connecting to
			data: {
				filename: name,
				Dcard: card
			},
		});
	});
	
	$('#editPropBtn').click(function(e){
		e.preventDefault();
		console.log("edit propery stub");
	});
	
});

function loadTable(filename){
	console.log("loading view table");
	
	$.ajax({
		type: 'get',            //Request type
		dataType: 'json',       //Data type - we will use JSON for almost everything 
		url: '/view',   //The server endpoint we are connecting to
		data: {
			Dfilename: filename
		},
		success: function(data){
			console.log(data.Dfilename);
			let CardString = data.Dfilename;
			let CardObj = JSON.parse(CardString);
			let FN = CardObj.FN;
			let optionalProperties = CardObj.optionalProperties;
			
			$("#viewTable tbody").empty();
			let i = 0;
			let j = 0;
			let numProp = 1;
			let name = "";
			let btext = "";
			let atext = "";
			
			
			for(j = 0; j < FN.values.length; j++){
				if(j == FN.values.length - 1){
					name = name + FN.values[j];
				}
				else{
					name = name + FN.values[j] + ",";
				}
			}
			
			if(CardObj.birthday){
				let birthday = CardObj.birthday;
				if(birthday.isText == true){
					btext = birthday.text;
				}
				else{
					btext = birthday.date;
					if(birthday.time != ""){
						btext = btext + "T" + birthday.time;
					}
				}
				numProp = numProp + 1;
			}
			
			if(CardObj.anniversary){
				let anniversary = CardObj.anniversary;
				if(anniversary.isText == true){
					atext = anniversary.text; 
				}
				else{
					atext = anniversary.date;
					if(anniversary.time != ""){
						atext = atext + "T" + anniversary.time;
					}
				}
				numProp = numProp + 1;
			}
			
			$('#viewBody').append("<tr>"+
			"<td>1</td>"+
			"<td>FN</td>"+
			"<td>"+name+"</td>"+
			"<td>0</td>"+
			"</tr>");	
			
			if(btext != ""){
				$('#viewBody').append("<tr>"+
				"<td>2</td>"+
				"<td>birthday</td>"+
				"<td>"+btext+"</td>"+
				"<td>0</td>"+
				"</tr>");
			}
			
			if(atext != ""){
				$('#viewBody').append("<tr>"+
				"<td>3</td>"+
				"<td>anniversary</td>"+
				"<td>"+atext+"</td>"+
				"<td>0</td>"+
				"</tr>");
			}
			
			
			for(i = 0; i < optionalProperties.length; i++){
				numProp = numProp + 1;
				let values = "";
				for(j = 0; j < optionalProperties[i].values.length; j++){
					if(j == optionalProperties[i].values.length - 1){
						values = values + optionalProperties[i].values[j];
					}
					else{
						values = values + optionalProperties[i].values[j] + ",";
					}
				}
				$('#viewBody').append("<tr>"+
				"<td>"+numProp+"</td>"+
				"<td>"+optionalProperties[i].name+"</td>"+
				"<td>"+values+"</td>"+
				"<td>0</td>"+
				"</tr>");
			}
			console.log("table loaded");
			$('#editMe').html("Currently editing: "+ filename);
			console.log("target made for editing");
		}
	});
}

