import discord
from discord.ext import commands
import asyncio

intents = discord.Intents.default()
intents.message_content = True

bot = commands.Bot(command_prefix="!", intents=intents)

@bot.event
async def on_ready():
    print(f"Bot online come {bot.user}")

async def load_extensions():
    await bot.load_extension("commands.ip_lookup")
    await bot.load_extension("commands.domain_lookup")
    await bot.load_extension("commands.breach_check")
    await bot.load_extension("commands.headers_scan")
    await bot.load_extension("commands.phone_lookup")
    await bot.load_extension("commands.social_scan")
    await bot.load_extension("commands.portscan")
    await bot.load_extension("commands.whois_lookup")

async def main():
    async with bot:
        await load_extensions()
        await bot.start("MTQ1NTU0NTg4OTE0MDUxMDg0MQ.GXGAyQ.lrMevozQDGtI7qjzUTBGyNwrN5pbL6c4lxOP90")

asyncio.run(main())
