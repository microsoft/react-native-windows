var builder = WebApplication.CreateBuilder(args);
builder.WebHost.UseUrls("http://localhost:7777");

var app = builder.Build();
//app.MapGet("/", () => "Hello World!");
app.UseWebSockets();

app.Map("/http0", async context => { });
app.Map("/ws1", async context =>
{

});

//app.Run();
await app.RunAsync();
