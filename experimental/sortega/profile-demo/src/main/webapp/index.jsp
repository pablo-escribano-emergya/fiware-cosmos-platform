<%@page contentType="text/html" pageEncoding="UTF-8"%>
<html>
    <head>
        <title>API demo</title>
        <script type="text/javascript">
            function do_lookup() {
                var ajaxRequest = new XMLHttpRequest();
                ajaxRequest.onreadystatechange = function(){
                    if(ajaxRequest.readyState == 4){
                        var ajaxDisplay = document.getElementById('categories');
                        ajaxDisplay.innerHTML = ajaxRequest.responseText;
                    }
                }
                ajaxRequest.open("GET", "api/?user=" + 
                    document.getElementById('client').value, true);
                ajaxRequest.send(null);                
            }
            
        </script>
    </head>

    <body>
        <h1>API demo</h1>

            <label for="client">Client</label>
            <select id="client" name="client">
                <option value="b9980e15c439fe91">b9980e15c439fe91</option>
                <option value="b8ce2743ea776de0">b8ce2743ea776de0</option> 
                <option value="b8cf82e1c98c52f7">b8cf82e1c98c52f7</option> 
                <option value="b8d01ab20a89bc54">b8d01ab20a89bc54</option> 
                <option value="b8d36d1f4d7eb522">b8d36d1f4d7eb522</option> 
                <option value="b8d39db3316caa34">b8d39db3316caa34</option> 
                <option value="b8d3ef4b5b141df1">b8d3ef4b5b141df1</option> 
                <option value="b8d48035ffe4d344">b8d48035ffe4d344</option> 
                <option value="b8d494d8967f821a">b8d494d8967f821a</option> 
                <option value="Z4455">Z4455</option> 
                <option value="X1234">X1234</option> 
            </select>
            <input type="submit" value="Lookup" onclick="do_lookup()" />

        <p>Categories: <span id="categories">-</span></p>
    </body>
</html>

