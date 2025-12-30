import aiohttp
import discord
from discord.ext import commands

class BreachCheck(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def breach(self, ctx, email: str):
        """Controlla se un'email è presente in data breach"""

        await ctx.send("🔍 Controllo in corso...")

        url = f"https://leakcheck.io/api/public?check={email}"

        try:
            async with aiohttp.ClientSession() as session:
                async with session.get(url) as response:
                    data = await response.json()
        except Exception as e:
            await ctx.send(f"❌ Errore durante la richiesta: `{e}`")
            return

        # Nessun risultato
        if not data.get("found"):
            embed = discord.Embed(
                title="🟢 Nessun Data Breach trovato",
                description=f"L'email **{email}** non risulta in nessun leak conosciuto.",
                color=discord.Color.green()
            )
            await ctx.send(embed=embed)
            return

        # Risultati trovati
        embed = discord.Embed(
            title=f"🔴 Data Breach trovati per {email}",
            color=discord.Color.red()
        )

        for entry in data.get("sources", [])[:10]:
            embed.add_field(
                name=f"🔓 Leak: {entry}",
                value="Password disponibile solo con API key",
                inline=False
            )

        embed.set_footer(text="Fonte: LeakCheck.io (public API)")
        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(BreachCheck(bot))