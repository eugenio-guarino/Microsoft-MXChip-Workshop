using IoTHubTrigger = Microsoft.Azure.WebJobs.EventHubTriggerAttribute;

using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Host;
using Microsoft.Azure.EventHubs;
using System.Text;
using System.Net.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

namespace IoTWorkbench
{
  public class TemperatureItem
  {
    [JsonProperty("id")]
    public string Id {get; set;}
    public double Temperature {get; set;}
    public double Threshold {get; set;}
  }

  public static class IoTHubTrigger1
  {
    [FunctionName("IoTHubTrigger1")]
    public static void Run([IoTHubTrigger("%eventHubConnectionPath%",
                                          Connection = "eventHubConnectionString")] EventData message,
                           [CosmosDB(databaseName: "IoTData",
                                     collectionName: "Temperatures",
                                     ConnectionStringSetting = "cosmosDBConnectionString")] out TemperatureItem output,
                            ILogger log)
    {
      var deviceId = message.SystemProperties["iothub-connection-device-id"].ToString();

      var jsonBody = Encoding.UTF8.GetString(message.Body);
      dynamic data = JsonConvert.DeserializeObject(jsonBody);
      double temperature = data.temperature;
      double threshold = data.threshold;

      output = new TemperatureItem
      {
        Temperature = temperature,
        Threshold = threshold,
        Id = deviceId
      };
    }
  }
}

