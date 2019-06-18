// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.ObjectModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;

namespace PerfCompare
{
    public sealed partial class MarkupPage : Page
    {
        public ObservableCollection<Message> Messages => _messages ?? (_messages = LoadMessages(App.TotalMessages));
        private ObservableCollection<Message> _messages = null;

        public MarkupPage()
        {
            this.InitializeComponent();
        }

        private ObservableCollection<Message> LoadMessages(int count)
        {
            ObservableCollection<Message> messages = new ObservableCollection<Message>();

            for (int i = 0; i < count; i++)
            {
                messages.Add(Message.Create(i));
            }

            return messages;
        }

        private async void Image_Loaded(object sender, RoutedEventArgs e)
        {
            if (sender is Image img && (img.GetValue(AutomationProperties.AutomationIdProperty) as string) == $"m{App.TotalMessages}")
            {
                await Dispatcher.RunIdleAsync((args) =>
                {
                    App.PerfStats.Stop(true);
                    App.ShowStats("XAML Markup");
                });
            }
        }

        private async void Page_Loaded(object sender, RoutedEventArgs e)
        {
            await Dispatcher.RunIdleAsync((args) =>
            {
                App.PerfStats.Stop(false);
                App.ShowStats("XAML Markup");
            });
        }
    }

    public sealed class Message
    {
        public string MessageID { get; private set; }

        public string Text { get; private set; }

        public string UserName { get; private set; }

        public string Avatar { get; private set; }

        public DateTime Timestamp { get; private set; }

        public string Time => Timestamp.ToString("h:mm tt");

        private Message() { }

        public static Message Create(int id)
        {
            return new Message
            {
                MessageID = "m" + (id + 1),
                Text = GetValue(id, TextValues),
                UserName = GetValue(id, UserNameValues),
                Avatar = GetValue(id, AvatarValues),
                Timestamp = DateTime.Now.AddMinutes(id),
            };
        }

        private static string GetValue(int index, string[] values)
        {
            return values[index % values.Length];
        }

        private static string[] TextValues = new string[]
        {
            "What time did the man go to the dentist?",
            "I don't know.",
            "Tooth hurt-y.",
            "Did you hear about the guy who invented Lifesavers?",
            "No.",
            "They say he made a mint.",
            "A ham sandwich walks into a bar and orders a beer. Bartender says, 'Sorry we don't serve food here.'",
            "Whenever the cashier at the grocery store asks my dad if he would like the milk in a bag he replies, 'No, just leave it in the carton!'",
            "Why do chicken coops only have two doors?",
            "I don't know, why?",
            "Because if they had four, they would be chicken sedans!",
        };

        private static string[] UserNameValues = new string[]
        {
            "Parent",
            "Child",
        };

        private static string[] AvatarValues = new string[]
        {
            "ms-appx:///images/blueuser.png",
            "ms-appx:///images/reduser.png",
        };
    }
}
