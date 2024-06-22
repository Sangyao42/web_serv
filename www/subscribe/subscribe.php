<?php
echo "Content-Type: application/json\r\n\r\n";

// Get the raw POST data
$rawData = file_get_contents("php://input");
// Decode the JSON data
$data = json_decode($rawData, true);

    // Check if email is set and valid
if (isset($data['email'])) {
    $email = filter_var($data['email'], FILTER_SANITIZE_EMAIL);

    if (filter_var($email, FILTER_VALIDATE_EMAIL)) {
        // Save the email into database
        // File path to save the email addresses
        $docRoot = $_SERVER['DOCUMENT_ROOT'];
        $filePath = $docRoot . '/emails/emails.txt';

        // Open the file in append mode
        $file = fopen($filePath, 'a');
        
        if ($file == false)
        {
            $response = array(
                "status" => "error",
                "message" => "Error while opening the file."
            );
            echo json_encode($response);
            exit(1);
        }
        
        fwrite($file, $email . PHP_EOL);

        if (!fclose($file))
        {
            $response = array(
                "status" => "error",
                "message" => "Error while closing the file."
            );
            echo json_encode($response);
            exit(1);
        };

        // The subscription is successful
        $response = array(
            echo "Content-Type: application/json\r\n\r\n",
            "status" => "success",
            "message" => "You have been subscribed successfully!",
            "headers" => "Content-Type: text/html\r\n\r\n"
        );
    } else {
        $response = array(
            "status" => "error",
            "message" => "Invalid email format."
        );
    }
} else {
    $response = array(
        "status" => "error",
        "message" => "Email is required."
    );
}

// Return the response as JSON
echo json_encode($response);

?>
