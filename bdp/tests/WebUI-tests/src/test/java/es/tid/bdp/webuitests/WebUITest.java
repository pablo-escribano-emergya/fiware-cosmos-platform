package es.tid.bdp.webuitests;

import es.tid.bdp.joblaunchers.CosmosWebUITester;
import es.tid.bdp.joblaunchers.TaskStatus;
import es.tid.bdp.joblaunchers.TestException;
import es.tip.bdp.frontend_om.FrontEnd;
import es.tip.bdp.frontend_om.SelectInputPage;
import es.tip.bdp.frontend_om.SelectJarPage;
import es.tip.bdp.frontend_om.SelectNamePage;
import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;
import java.util.UUID;
import junit.framework.Assert;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

public class WebUITest {
    private FrontEnd _frontend;
    private String WORDCOUNT_JAR_PATH;
    
    @BeforeClass
    public void setUp() {
        File wordCountJarFile = new File("wordcount.jar");
        WORDCOUNT_JAR_PATH = wordCountJarFile.getAbsolutePath();
        Assert.assertTrue("Veryfing Wordcount Jar is present: "
                + WORDCOUNT_JAR_PATH, wordCountJarFile.exists());
    }
    
    public WebUITest() {
        _frontend = new FrontEnd();
    }
       
    public static boolean isLive(String link) { 
        HttpURLConnection urlConnection = null; 
        try { 
            URL url = new URL(link); 
            urlConnection = (HttpURLConnection) url.openConnection(); 
            urlConnection.setRequestMethod("GET"); 
            urlConnection.connect(); 
            String redirectLink = urlConnection.getHeaderField("Location"); 
            if (redirectLink != null && !url.toString().equals(redirectLink)) { 
                return isLive(redirectLink); 
            }
            else { 
                return urlConnection.getResponseCode()
                        == HttpURLConnection.HTTP_OK; 
            } 
        }
        catch (Exception e) { 
            return false; 
        }
        finally { 
            if (urlConnection != null) {
                urlConnection.disconnect();
            }
        } 
    }
    
    private void verifyLinks() throws MalformedURLException {
        List<WebElement> links = _frontend.getDriver()
                                    .findElements(By.tagName("a"));
       
        for(WebElement link : links) {
            URL baseUrl = new URL(FrontEnd.HOME_URL);
            String verbatimUrl = link.getAttribute("href");
            String linkUrl = new URL(baseUrl, verbatimUrl).toString();
            Assert.assertTrue("Broken link: " + linkUrl, isLive(linkUrl));
        }
    }
    
    @Test
    public void testMainPage() throws MalformedURLException {
        _frontend.goHome();
        verifyLinks();
    }
    
    @Test
    public void testNoNameFile() throws MalformedURLException {
        WebDriver driver = _frontend.getDriver();
        SelectNamePage namePage = _frontend.goToCreateNewJob();
        
        String currentUrl = driver.getCurrentUrl();
        verifyLinks();
        
        namePage.submitNameForm();
        
        // We should be in the same page, and the form should be complaining
        Assert.assertEquals(currentUrl, driver.getCurrentUrl());
        Assert.fail(); // TODO: Need to verify that some error text has appeared
    }
    
    @Test
    public void testNoJarFile() throws MalformedURLException {
        WebDriver driver = _frontend.getDriver();
        SelectNamePage namePage = _frontend.goToCreateNewJob();
        
        namePage.setName(UUID.randomUUID().toString());
        SelectJarPage jarPage = namePage.submitNameForm();
        
        String currentUrl = driver.getCurrentUrl();
        verifyLinks();
        
        jarPage.submitJarFileForm();
        
        // We should be in the same page, and the form should be complaining
        Assert.assertEquals(currentUrl, driver.getCurrentUrl());
        Assert.fail(); // TODO: Need to verify that some error text has appeared
    }
    
    @Test
    public void testNoInputFile() throws IOException {
        WebDriver driver = _frontend.getDriver();
        SelectNamePage namePage = _frontend.goToCreateNewJob();
        
        namePage.setName(UUID.randomUUID().toString());
        SelectJarPage jarPage = namePage.submitNameForm();
        jarPage.setInputJar(WORDCOUNT_JAR_PATH);        
        SelectInputPage inputPage = jarPage.submitJarFileForm();
        
        String currentUrl = driver.getCurrentUrl();
        verifyLinks();
        
        inputPage.submitInputFileForm();
        
        // We should be in the same page, and the form should be complaining
        Assert.assertEquals(currentUrl, driver.getCurrentUrl());
        Assert.fail(); // TODO: Need to verify that some error text has appeared
    }
    
    @Test
    public void verifySampleJarFile() throws IOException {
        WebDriver driver = _frontend.getDriver();
        SelectNamePage namePage = _frontend.goToCreateNewJob();
        namePage.setName(UUID.randomUUID().toString());
        
        SelectJarPage jarPage = namePage.submitNameForm();
                
        final String sampleJarLinkId = "FakeId";
        driver.findElement(By.id(sampleJarLinkId));  // Just verifying it exists
        verifyLinks();
    }
    
    @Test
    public void verifyJarRestrictions() throws IOException {
        WebDriver driver = _frontend.getDriver();
        SelectNamePage namePage = _frontend.goToCreateNewJob();
        namePage.setName(UUID.randomUUID().toString());
        SelectJarPage jarPage = namePage.submitNameForm();
                
        final String jarRestrictionsId = "FakeId";
        String restrictions = driver.findElement(By.id(jarRestrictionsId))
                                   .getText();
        
        Assert.assertTrue(
                "Verifying restrictions mention HDFS",
                restrictions.contains("HDFS"));
        
        Assert.assertTrue(
                "Verifying restrictions mention Mongo",
                restrictions.contains("Mongo"));
        
        Assert.assertTrue(
                "Verifying restrictions mention manifests",
                restrictions.contains("manifest"));
        
        Assert.assertTrue(
                "Verifying restrictions mention the Tool interface",
                restrictions.contains("Tool"));
    }
    
    @Test
    public void testSimpleTask() throws IOException, TestException {
        final String inputFilePath;
        
        {
            File tmpFile = File.createTempFile("webui-wordcount", ".tmp");
            tmpFile.deleteOnExit();
            
            PrintWriter writer = new PrintWriter(tmpFile);
            try {
                writer.write("Very simple text file");
            }
            finally {
                writer.close();
            }
            
            inputFilePath = tmpFile.getAbsolutePath();
        }
        
        CosmosWebUITester testDriver = new CosmosWebUITester();
        String taskId = testDriver.createNewTask(inputFilePath,
                                                 WORDCOUNT_JAR_PATH);
        testDriver.waitForTaskCompletion(taskId);
        testDriver.getResults(taskId); // Just verifying results can be accessed
    }
    
    @Test
    public void testParallelTasks() throws IOException, TestException {
        final String inputFilePath;
        final int taskCount = 4;
        
        {
            File tmpFile = File.createTempFile("webui-wordcount", ".tmp");
            tmpFile.deleteOnExit();
            
            PrintWriter writer = new PrintWriter(tmpFile);
            try {
                writer.write("Very simple text file");
            }
            finally {
                writer.close();
            }
            
            inputFilePath = tmpFile.getAbsolutePath();
        }
        
        CosmosWebUITester testDriver = new CosmosWebUITester();
        
        String[] taskIds = new String[taskCount];
        for(int i = 0; i < taskCount; ++i) {
            taskIds[i] = testDriver.createNewTask(inputFilePath,
                                                 WORDCOUNT_JAR_PATH,
                                                 false);
        }
        
        for(int i = 0; i < taskCount; ++i) {
            Assert.assertEquals("Veryfing task is in created state. TaskId: "
                                + taskIds[i],
                                TaskStatus.Created,
                                testDriver.getTaskStatus(taskIds[i]));
        }
        
        for(int i = 0; i < taskCount; ++i) {
            testDriver.launchTask(taskIds[i]);
        }
        
        for(int i = 0; i < taskCount; ++i) {
            TaskStatus jobStatus = testDriver.getTaskStatus(taskIds[i]);
            Assert.assertTrue("Verifying task is in running or completed state."
                                + " TaskId: " + taskIds[i],
                              jobStatus == TaskStatus.Completed ||
                              jobStatus == TaskStatus.Running);
        }
        
        for(int i = 0; i < taskCount; ++i) {
            testDriver.waitForTaskCompletion(taskIds[i]);
        }
        
        for(int i = 0; i < taskCount; ++i) {
            TaskStatus jobStatus = testDriver.getTaskStatus(taskIds[i]);
            Assert.assertEquals("Verifying task is in completed state."
                                + " TaskId: " + taskIds[i],
                                jobStatus,
                                TaskStatus.Completed);
        }
        
        for(int i = 0; i < taskCount; ++i) {
            // Just verifying results can be accessed
            testDriver.getResults(taskIds[i]);
        }
    }
}
