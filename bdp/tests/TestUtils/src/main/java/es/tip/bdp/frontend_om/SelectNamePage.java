package es.tip.bdp.frontend_om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.testng.Assert;

public class SelectNamePage {
    public static final String NAME_ID = "job-name";
    
    private final String selectNameUrl;
    private WebDriver driver;
    
    private void assertCorrectUrl() {
        Assert.assertEquals(this.driver.getCurrentUrl(), this.selectNameUrl);
    }
    
    public SelectNamePage(WebDriver driver) {
        this.driver = driver;
        this.selectNameUrl = driver.getCurrentUrl();
    }
    
    public void setName(String taskName) {
        assertCorrectUrl();
        WebElement nameElement = this.driver.findElement(By.id(NAME_ID));
        nameElement.sendKeys(taskName);
    }
    
    public SelectJarPage submitNameForm() {
        assertCorrectUrl();
        WebElement nameElement = this.driver.findElement(By.id(NAME_ID));
        nameElement.submit();
        return new SelectJarPage(this.driver);
    }
}
